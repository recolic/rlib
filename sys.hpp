#ifndef R_SYS_HPP
#define R_SYS_HPP
#include <unistd.h>
#include <sys/type.h>
#include "rstdafx.hpp"
_recolic_hpp_begin
class FileDescriptorSet
{
public:
    FileDescriptorSet() : m_size(0), maxFileDescriptor(NULL) {FD_ZERO(&m_fds_data);}
    void push(HANDLE FileDescriptor) {FD_SET(FileDescriptor, &m_fds_data); ++m_size; maxFileDescriptor = (maxFileDescriptor > FileDescriptor ? maxFileDescriptor : FileDescriptor);}
    void pop(HANDLE FileDescriptor) {FD_CLR(FileDescriptor, &m_fds_data); --m_size;} //It will break maxFileDescriptor.(for performance reason).
    void clear() {FD_ZERO(&m_fds_data); m_size = 0;maxFileDescriptor = 0;}
    bool check(HANDLE FileDescriptor) {return FD_ISSET(FileDescriptor, &m_fds_data);}
    size_t size() const {return m_size;}
    int getMaxFileDescriptor() const {return maxFileDescriptor;}
    fd_set *getptr() {return &m_fds_data;}
private:
    fd_set m_fds_data;
    size_t m_size;
    int maxFileDescriptor;
};

#include <pthread.h>
class SRWLock
{
public:
    SRWLock() : isFree(true) {pthread_rwlock_init(&m_lock, NULL);}
    ~SRWLock() {pthread_rwlock_destroy(&m_lock);}
    void acquireShared() {pthread_rwlock_rdlock(&m_lock);isFree = false;}
    void acquireExclusive() {pthread_rwlock_wrlock(&m_lock);isFree = false;}
    void release() {pthread_rwlock_unlock(&m_lock);isFree = true;}
//    bool tryAcquireShared() {return pthread_rwlock_tryrdlock(&m_lock) == 0;}
//    bool tryAcquireExclusive() {return pthread_rwlock_trywrlock(&m_lock) == 0;}
private:
    pthread_rwlock_t m_lock;
    bool isFree;
};
_recolic_hpp_end

#endif