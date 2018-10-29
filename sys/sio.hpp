#ifndef R_SIO_HPP
#define R_SIO_HPP

#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <stdexcept>

#ifndef WIN32
#include <sys/socket.h>
//POSIX Version
namespace rlib {
    namespace impl {
        inline void MakeNonBlocking(int fd) {
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

    static inline fd quick_listen(const std::string &addr, uint16_t port) {
        addrinfo *psaddr;
        addrinfo hints{0};
        fd listenfd;

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

    static inline fd quick_connect(const std::string &addr, uint16_t port) {
        addrinfo *paddr;
        addrinfo hints{0};
        fd sockfd;

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
        static ssize_t readn(int fd, void *vptr, size_t n) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
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
        static ssize_t writen(int fd, const void *vptr, size_t n) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
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
        static ssize_t readall(int fd, void **pvptr, size_t initSize) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
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
        static void readn_ex(int fd, void *vptr, size_t n) //never return error.
        {
            auto ret = readn(fd, vptr, n);
            if(ret == -1) throw std::runtime_error("readn failed.");
        }
        static void writen_ex(int fd, const void *vptr, size_t n)
        {
            auto ret = writen(fd, vptr, n);
            if(ret == -1) throw std::runtime_error("writen failed.");
        }
        static ssize_t readall_ex(int fd, void **pvptr, size_t initSize) //never return -1
        {
            auto ret = readall(fd, pvptr, initSize);
            if(ret == -1) throw std::runtime_error("readall failed.");
            return ret;
        }
    };

    class sockIO 
    {
    public:
        static ssize_t recvn(int fd, void *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
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
        static ssize_t sendn(int fd, const void *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
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
        static ssize_t recvall(int fd, void **pvptr, size_t initSize, int flags) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
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
        static void recvn_ex(int fd, void *vptr, size_t n, int flags) //return read bytes.
        {
            auto ret = recvn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("recvn failed.");
        }
        static ssize_t sendn_ex(int fd, const void *vptr, size_t n, int flags)
        {
            auto ret = sendn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("sendn failed.");
            return ret;
        }
        static ssize_t recvall_ex(int fd, void **pvptr, size_t initSize, int flags) //never return -1
        {
            auto ret = recvall(fd, pvptr, initSize, flags);
            if(ret == -1) throw std::runtime_error("recvall failed.");
            return ret;
        }
    };
}
#else
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
//WINsock version
namespace rlib {
    template <bool doNotWSAStartup = false>
    static inline fd quick_listen(const std::string &addr, uint16_t port) {
    {
        WSADATA wsaData;
        SOCKET listenfd = INVALID_SOCKET;
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
    static inline fd quick_connect(const std::string &addr, uint16_t port) {
    {
        WSADATA wsaData;
        SOCKET listenfd = INVALID_SOCKET;
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
        static ssize_t recvn(SOCKET fd, char *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
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
        static ssize_t sendn(SOCKET fd, const char *vptr, size_t n, int flags) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
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
        static ssize_t recvall(SOCKET fd, void **pvptr, size_t initSize, int flags) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
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
        static void recvn_ex(SOCKET fd, char *vptr, size_t n, int flags) //never return error.
        {
            auto ret = recvn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("recvn failed.");
        }
        static ssize_t sendn_ex(SOCKET fd, const char *vptr, size_t n, int flags)
        {
            auto ret = sendn(fd, vptr, n, flags);
            if(ret == -1) throw std::runtime_error("recvn failed.");
						return ret;
        }
        static ssize_t recvall_ex(SOCKET fd, void **pvptr, size_t initSize, int flags) //never return -1
        {
            auto ret = recvall(fd, pvptr, initSize, flags);
            if(ret == -1) throw std::runtime_error("recvn failed.");
            return ret;
        }
    };

    class fdIO
    {
    public:
        static ssize_t readn(SOCKET fd, void *vptr, size_t n) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            return sockIO::recvn(fd, (char *)vptr, n, 0);
        }
        static ssize_t writen(SOCKET fd, const void *vptr, size_t n) noexcept //Return -1 on error, read bytes on success, blocks until nbytes done.
        {
            return sockIO::sendn(fd, (const char *)vptr, n, 0);
        }
        static ssize_t readall(SOCKET fd, void **pvptr, size_t initSize) noexcept //Return -1 on error, read bytes on success. pvptr must be a malloc/calloced buffer, I'll malloc one if *pvptr is NULL.
        {
            return sockIO::recvall(fd, pvptr, initSize, 0);
        }
        static void readn_ex(SOCKET fd, void *vptr, size_t n) //return read bytes.
        {
            return sockIO::recvn_ex(fd, (char *)vptr, n, 0);
        }
        static ssize_t writen_ex(SOCKET fd, const void *vptr, size_t n)
        {
            return sockIO::sendn_ex(fd, (const char *)vptr, n, 0);
        }
        static ssize_t readall_ex(SOCKET fd, void **pvptr, size_t initSize) //never return -1
        {
            return sockIO::recvall_ex(fd, pvptr, initSize, 0);
        }
    };
}

#endif


#endif
