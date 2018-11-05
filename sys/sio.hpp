#ifndef R_SIO_HPP
#define R_SIO_HPP

#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <rlib/sys/fd.hpp>
#include <rlib/sys/os.hpp>
#include <rlib/string.hpp>
#include <rlib/scope_guard.hpp>

#if RLIB_OS_ID == OS_WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#endif


namespace rlib {
    // Both POSIX and Win32
    static inline sockfd_t quick_accept(sockfd_t sock) {
        return accept(sock, NULL, NULL);
    }
}

#if RLIB_OS_ID != OS_WINDOWS
//POSIX Version
namespace rlib {
    using rlib::literals::operator "" _format;
    namespace impl {
        inline void MakeNonBlocking(fd_t fd) {
            int flags, s;

            flags = fcntl (fd, F_GETFL, 0);
            if (flags == -1) {
                perror ("fcntl");
                exit(-1);
            }

            flags |= O_NONBLOCK;
            s = fcntl (fd, F_SETFL, flags);
            if (s == -1) {
                perror ("fcntl");
                exit(-1);
            }
        }
    }

    static inline fd_t quick_listen(const std::string &addr, uint16_t port) {
        addrinfo *psaddr;
        addrinfo hints{0};
        fd_t listenfd;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        auto _ = getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hints, &psaddr);
        if (_ != 0) throw std::runtime_error("Failed to getaddrinfo. returnval={}, check `man getaddrinfo`'s return value."_format(_));

        bool success = false;
        for (addrinfo *rp = psaddr; rp != nullptr; rp = rp->ai_next) {
            listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (listenfd == -1)
                continue;
            int reuse = 1;
            if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(int)) < 0)
                throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
            if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const char *) &reuse, sizeof(int)) < 0)
                throw std::runtime_error("setsockopt(SO_REUSEPORT) failed");
            if (bind(listenfd, rp->ai_addr, rp->ai_addrlen) == 0) {
                success = true;
                break;
            }
            close(listenfd);
        }
        if (!success) throw std::runtime_error("Failed to bind {}:{}."_format(addr, port));

        if (-1 == ::listen(listenfd, 16)) throw std::runtime_error("listen failed.");

        rlib_defer([psaddr] { freeaddrinfo(psaddr); });
        return listenfd;
    }

    static inline fd_t quick_connect(const std::string &addr, uint16_t port) {
        addrinfo *paddr;
        addrinfo hints{0};
        fd_t sockfd;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        auto _ = getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hints, &paddr);
        if (_ != 0)
            throw std::runtime_error("getaddrinfo failed. Check network connection to {}:{}; returnval={}, check `man getaddrinfo`'s return value."_format(
                    addr.c_str(), port, _));
        rlib_defer([paddr] { freeaddrinfo(paddr); });

        bool success = false;
        for (addrinfo *rp = paddr; rp != NULL; rp = rp->ai_next) {
            sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sockfd == -1)
                continue;
            int reuse = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(int)) < 0)
                throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char *) &reuse, sizeof(int)) < 0)
                throw std::runtime_error("setsockopt(SO_REUSEPORT) failed");
            if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
                success = true;
                break; /* Success */
            }
            close(sockfd);
        }
        if (!success) throw std::runtime_error("Failed to connect to any of these addr.");

        return sockfd;
    }

    class fdIO
    {
    public:
        static ssize_t readn(fd_t fd, void *vptr, size_t n) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            size_t  nleft;
            ssize_t nread;
            char   *ptr;
        
            ptr = (char *)vptr;
            nleft = n;
            while (nleft > 0) {
                if ( (nread = read(fd, ptr, nleft)) < 0) {
                    if (errno == EINTR)
                        nread = 0;      /* and call read() again */
                    else
                        return (-1);
                } else if (nread == 0)
                    return (-1);              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
            return (n);         /* return success */
        }
        static ssize_t writen(fd_t fd, const void *vptr, size_t n) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            size_t nleft;
            ssize_t nwritten;
            const char *ptr;
        
            ptr = (const char *)vptr;
            nleft = n;
            while (nleft > 0) {
                if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
                    if (nwritten < 0 && errno == EINTR)
                        nwritten = 0;   /* and call write() again */
                    else
                        return (-1);    /* error */
                 }
             
                 nleft -= nwritten;
                 ptr += nwritten;
            }
            return (n);
        }
        static ssize_t readall(fd_t fd, void **pvptr, size_t initSize) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
        {
            size_t current = initSize ? initSize : 1024;
            void *vptr = *pvptr;
            if(vptr == NULL)
                vptr = malloc(current);
            void *currvptr = vptr;
        
            {
                ssize_t ret = read(fd, currvptr, current / 2);
                if(ret == -1) return -1;
                if(ret < current / 2)
                {
                    *pvptr = vptr;
                    return ret;
                }
                currvptr = (char *)vptr + current / 2;
            }
        
            while(true)
            {
                ssize_t ret = read(fd, currvptr, current / 2);
                if(ret == -1) return -1;
                if(ret < current)
                {
                    *pvptr = vptr;
                    return ret + current / 2;
                }
            
                current *= 2;
                void *vptrBackup = vptr;
                if((vptr = realloc(vptr, current)) == NULL) {
                    free(vptrBackup);
                    errno = EMSGSIZE;
                    return -1;
                }
                currvptr = (char *)vptr + current / 2;
            }
        }
        static void readn_ex(fd_t fd, void *vptr, size_t n) //never return error.
        {
            auto ret = readn(fd, vptr, n);
            if(ret == -1) throw std::runtime_error("readn failed.");
        }
        static void writen_ex(fd_t fd, const void *vptr, size_t n)
        {
            auto ret = writen(fd, vptr, n);
            if(ret == -1) throw std::runtime_error("writen failed.");
        }
        static ssize_t readall_ex(fd_t fd, void **pvptr, size_t initSize) //never return -1
        {
            auto ret = readall(fd, pvptr, initSize);
            if(ret == -1) throw std::runtime_error("readall failed.");
            return ret;
        }
        static std::string quick_readn(fd_t fd, size_t n) {
            std::string res(n, '0');
            readn_ex(fd, (void *)res.data(), n);
            return res;
        }
        static std::string quick_readall(fd_t fd) {
            void *ptr;
            auto size = readall_ex(fd, &ptr, NULL);
            return std::string((char *)ptr, size);
        }
        static void quick_writen(fd_t fd, const std::string &data) {
            writen_ex(fd, data.data(), data.size());
        }
    };

    class sockIO 
    {
    public:
        static ssize_t recvn(fd_t fd, void *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            size_t  nleft;
            ssize_t nread;
            char   *ptr;
        
            ptr = (char *)vptr;
            nleft = n;
            while (nleft > 0) {
                if ( (nread = recv(fd, ptr, nleft, flags)) < 0) {
                    if (errno == EINTR)
                        nread = 0;      /* and call read() again */
                    else
                        return (-1);
                } else if (nread == 0)
                    return -1;              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
            return (n);         /* return success */
        }
        static ssize_t sendn(fd_t fd, const void *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            size_t nleft;
            ssize_t nwritten;
            const char *ptr;
        
            ptr = (const char *)vptr;
            nleft = n;
            while (nleft > 0) {
                if ( (nwritten = send(fd, ptr, nleft, flags)) <= 0) {
                    if (nwritten < 0 && errno == EINTR)
                        nwritten = 0;   /* and call write() again */
                    else
                        return (-1);    /* error */
                 }
             
                 nleft -= nwritten;
                 ptr += nwritten;
            }
            return (n);
        }
        static ssize_t recvall(fd_t fd, void **pvptr, size_t initSize, int flags) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
        {
            size_t current = initSize ? initSize : 1024;
            void *vptr = *pvptr;
            if(vptr == NULL)
                vptr = malloc(current);
            void *currvptr = vptr;
        
            {
                ssize_t ret = recv(fd, currvptr, current / 2, flags);
                if(ret == -1) return -1;
                if(ret < current / 2)
                {
                    *pvptr = vptr;
                    return ret;
                }
                currvptr = (char *)vptr + current / 2;
            }
        
            while(true)
            {
                ssize_t ret = recv(fd, currvptr, current / 2, flags);
                if(ret == -1) return -1;
                if(ret < current)
                {
                    *pvptr = vptr;
                    return ret + current / 2;
                }
            
                current *= 2;
                void *vptrBackup = vptr;
                if((vptr = realloc(vptr, current)) == NULL) {
                    free(vptrBackup);
                    errno = EMSGSIZE;
                    return -1;
                }
                currvptr = (char *)vptr + current / 2;
            }
        }
        static void recvn_ex(fd_t fd, void *vptr, size_t n, int flags) //return read bytes.
        {
            auto ret = recvn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("recvn failed.");
        }
        static void sendn_ex(fd_t fd, const void *vptr, size_t n, int flags)
        {
            auto ret = sendn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("sendn failed.");
        }
        static ssize_t recvall_ex(fd_t fd, void **pvptr, size_t initSize, int flags) //never return -1
        {
            auto ret = recvall(fd, pvptr, initSize, flags);
            if(ret == -1) throw std::runtime_error("recvall failed.");
            return ret;
        }
        static std::string quick_recvn(fd_t fd, size_t n) {
            std::string res(n, '0');
            recvn_ex(fd, (void *)res.data(), n, MSG_NOSIGNAL);
            return res;
        }
        static std::string quick_recvall(fd_t fd) {
            void *ptr;
            auto size = recvall_ex(fd, &ptr, NULL, MSG_NOSIGNAL);
            return std::string((char *)ptr, size);
        }
        static void quick_sendn(fd_t fd, const std::string &data) {
            sendn_ex(fd, data.data(), data.size(), MSG_NOSIGNAL);
        }
 
    };
}
#else
//WINsock version
namespace rlib {
    template <bool doNotWSAStartup = false>
    static inline sockfd_t quick_listen(const std::string &addr, uint16_t port) {
    {
        WSADATA wsaData;
        sockfd_t listenfd = INVALID_SOCKET;
        if(!doNotWSAStartup) {
            int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
            if (iResult != 0) throw std::runtime_error("WSAStartup failed with error: {}\n"_format(iResult));
        }
    
        addrinfo *psaddr;
        addrinfo hints { 0 };
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        hints.ai_protocol = IPPROTO_TCP;
        auto _ = getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hints, &psaddr);
        if(_ != 0) {
            WSACleanup();
            throw std::runtime_error("Failed to getaddrinfo. returnval={}, check `man getaddrinfo`'s return value."_format(_));
        }
    
        bool success = false;
        for (addrinfo *rp = psaddr; rp != NULL; rp = rp->ai_next) {
            listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (listenfd == INVALID_SOCKET)
                continue;
            int reuse = 1;
            if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) < 0) throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
            if (bind(listenfd, rp->ai_addr, rp->ai_addrlen) != SOCKET_ERROR) {
                success = true;
                break; /* Success */
            }
            closesocket(listenfd);
        }
        if(!success) throw std::runtime_error("Failed to bind to any of these addr.");
    
        if(SOCKET_ERROR == ::listen(listenfd, 16)) throw std::runtime_error("listen failed.");
    
        freeaddrinfo(psaddr);
        return listenfd;
    }

    template <bool doNotWSAStartup = false>
    static inline sockfd_t quick_connect(const std::string &addr, uint16_t port) {
    {
        WSADATA wsaData;
        sockfd_t listenfd = INVALID_SOCKET;
        if(!doNotWSAStartup) {
            int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
            if (iResult != 0) throw std::runtime_error("WSAStartup failed with error: {}\n"_format(iResult));
        }
    
        addrinfo *paddr;
        addrinfo hints { 0 };
    
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        auto _ = getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hints, &paddr);
        if(_ != 0) {
            WSACleanup();
            throw std::runtime_error("getaddrinfo failed. Check network connection to {}:{}; returnval={}, check `man getaddrinfo`'s return value."_format(serverIp.c_str(), serverPort, _));
        }
        rlib_defer([p=paddr]{WSACleanup();freeaddrinfo(p);});
    
        bool success = false;
        for (addrinfo *rp = paddr; rp != NULL; rp = rp->ai_next) {
            sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sockfd == INVALID_SOCKET)
                continue;
            int reuse = 1;
            if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) < 0) throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
            if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != SOCKET_ERROR) {
                success = true;
                break; /* Success */
            }
            closesocket(sockfd);
        }   
        if(!success) throw std::runtime_error("Failed to connect to any of these addr.");
    
        freeaddrinfo(paddr);
        return sockfd;
    }

    class sockIO 
    {
    private:
        static int WSASafeGetLastError()
        {
            int i;
            WSASetLastError(i = WSAGetLastError());
            return i;
        }
    public:
        static ssize_t recvn(sockfd_t fd, char *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            size_t  nleft;
            ssize_t nread;
            char   *ptr;
        
            ptr = (char *)vptr;
            nleft = n;
            while (nleft > 0) {
                if ( (nread = recv(fd, ptr, nleft, flags)) == SOCKET_ERROR) {
                    if (WSASafeGetLastError() == WSAEINTR)
                        nread = 0;      /* and call read() again */
                    else
                        return (-1);
                } else if (nread == 0)
                    return (-1);              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
            return (n);         /* return >= 0 */
        }
        static ssize_t sendn(sockfd_t fd, const char *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            size_t nleft;
            ssize_t nwritten;
            const char *ptr;
        
            ptr = (const char *)vptr;
            nleft = n;
            while (nleft > 0) {
                if ( (nwritten = send(fd, ptr, nleft, flags)) <= 0) {
                    if (nwritten == SOCKET_ERROR && WSASafeGetLastError() == WSAEINTR)
                        nwritten = 0;   /* and call write() again */
                    else
                        return (-1);    /* error */
                 }
             
                 nleft -= nwritten;
                 ptr += nwritten;
            }
            return (n);
        }
        static ssize_t recvall(sockfd_t fd, void **pvptr, size_t initSize, int flags) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
        {
            size_t current = initSize ? initSize : 1024;
            void *vptr = *pvptr;
            if(vptr == NULL)
                vptr = malloc(current);
            void *currvptr = vptr;
        
            {
            _retry_1:
                ssize_t ret = recv(fd, (char *)currvptr, current / 2, flags); 
                if(ret == SOCKET_ERROR) {
                    if(WSASafeGetLastError() == WSAEINTR)
                        goto _retry_1;
                    return SOCKET_ERROR;
                }
                if(ret < current / 2)
                {
                    *pvptr = vptr;
                    return ret;
                }
                currvptr = (char *)vptr + current / 2;
            }   
        
            while(true)
            {
                ssize_t ret = recv(fd, (char *)currvptr, current / 2, flags); 
                if(ret == SOCKET_ERROR) {
                    if(WSASafeGetLastError() == WSAEINTR)
                        continue; //retry
                    return SOCKET_ERROR;
                }
                if(ret < current)
                {
                    *pvptr = vptr;
                    return ret + current / 2;
                }
        
                current *= 2;
                void *vptrBackup = vptr;
                if((vptr = realloc(vptr, current)) == NULL) {
                    free(vptrBackup);
                    WSASetLastError(WSAEMSGSIZE);
                    return SOCKET_ERROR;
                }   
                currvptr = (char *)vptr + current / 2;
            }
        }
        static void recvn_ex(sockfd_t fd, char *vptr, size_t n, int flags) //never return error.
        {
            auto ret = recvn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("recvn failed.");
        }
        static void sendn_ex(sockfd_t fd, const char *vptr, size_t n, int flags)
        {
            auto ret = sendn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("sendn failed.");
        }
        static ssize_t recvall_ex(sockfd_t fd, void **pvptr, size_t initSize, int flags) //never return -1
        {
            auto ret = recvall(fd, pvptr, initSize, flags);
            if(ret == -1) throw std::runtime_error("recvall failed.");
            return ret;
        }
        static std::string quick_recvn(sockfd_t fd, size_t n) {
            std::string res(n, '0');
            recvn_ex(fd, (void *)res.data(), n, NULL);
            return res;
        }
        static std::string quick_recvall(fd_t fd) {
            void *ptr;
            auto size = recvall_ex(fd, &ptr, NULL, NULL);
            return std::string((char *)ptr, size);
        }
        static void quick_sendn(sockfd_t fd, const std::string &data) {
            sendn_ex(fd, data.data(), data.size(), NULL);
        }
    };

}

#endif


#endif
