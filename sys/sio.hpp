#ifndef R_SIO_HPP
#define R_SIO_HPP

#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <stdexcept>
namespace rlib {
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
                    break;              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
            return (n - nleft);         /* return >= 0 */
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
        static void readn_ex(int fd, void *vptr, size_t n) //with exception, never return error.
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
                        throw std::runtime_error("Readn failed with errno=" + std::to_string(errno));
                } else if (nread == 0)
                    break;              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
        }
        static void writen_ex(int fd, const void *vptr, size_t n)
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
                        throw std::runtime_error("Writen failed with errno=" + std::to_string(errno));
                 }
             
                 nleft -= nwritten;
                 ptr += nwritten;
            }
        }
        static ssize_t readall_ex(int fd, void **pvptr, size_t initSize) //never return -1
        {
            size_t current = initSize ? initSize : 1024;
            void *vptr = *pvptr;
            if(vptr == NULL)
                vptr = malloc(current);
            void *currvptr = vptr;
        
            {
                ssize_t ret = read(fd, currvptr, current / 2);
                if(ret == -1) throw std::runtime_error("read failed. errno=" + std::to_string(errno));
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
                if(ret == -1) throw std::runtime_error("read failed. errno=" + std::to_string(errno));
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
                    throw std::runtime_error("realloc failed. errno=" + std::to_string(errno));
                }
                currvptr = (char *)vptr + current / 2;
            }
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
                    break;              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
            return (n - nleft);         /* return >= 0 */
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
        static void recvn_ex(int fd, void *vptr, size_t n, int flags) //with exception, never return error.
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
                        throw std::runtime_error("Recvn failed with errno=" + std::to_string(errno));
                } else if (nread == 0)
                    break;              /* EOF */
            
                nleft -= nread;
                ptr += nread;
            }
        }
        static void sendn_ex(int fd, const void *vptr, size_t n, int flags)
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
                        throw std::runtime_error("Sendn failed with errno=" + std::to_string(errno));
                 }
             
                 nleft -= nwritten;
                 ptr += nwritten;
            }
        }
        static ssize_t recvall_ex(int fd, void **pvptr, size_t initSize, int flags) //never return -1
        {
            size_t current = initSize ? initSize : 1024;
            void *vptr = *pvptr;
            if(vptr == NULL)
                vptr = malloc(current);
            void *currvptr = vptr;
        
            {
                ssize_t ret = recv(fd, currvptr, current / 2, flags);
                if(ret == -1) throw std::runtime_error("recv failed. errno=" + std::to_string(errno));
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
                if(ret == -1) throw std::runtime_error("recv failed. errno=" + std::to_string(errno));
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
                    throw std::runtime_error("realloc failed. errno=" + std::to_string(errno));
                }
                currvptr = (char *)vptr + current / 2;
            }
        }
    };
}
#endif
