/*
Usage: Put this in /usr/bin/stdio.h

#if __has_include("/home/recolic/sh/rdebug.h")
#include "/home/recolic/sh/rdebug.h"
#endif

DO NOT modify this file directly. Please commit any change to ~/sh/rdebug.h
version: 1.0.3

changelog: 1.0.3: add print_time for c
*/


#ifndef RDB__H
#define RDB__H
#include <execinfo.h>
#include <stdio.h>
#include <stddef.h>
#define RDEBUG(fmt, ...) fprintf(stderr, "RDEBUG: %s:%d(%s), " #fmt "\n" , __FILE__, __LINE__, __func__ __VA_OPT__(,) __VA_ARGS__ );

#ifdef __cplusplus
#include <ostream>
#include <iomanip>
#include <cstdint>

[[maybe_unused]] inline
#else
__attribute__((unused)) static
#endif
void printbt() {
    void *array[32];
    int size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 32);

    // print out all the frames to stderr
    backtrace_symbols_fd(array, size, 2);
}


#ifdef __cplusplus
template <typename CharT>
[[maybe_unused]] inline void print_buf(std::ostream& out, const char *title, const CharT *data, size_t dataLen) {
    out << title << std::endl;
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (0x000000ff & (int32_t)(((const char *)data)[i]));
        // format
        out << (((i + 1) % 16 == 0) ? "\n" : " ");
    }
    out << std::endl;
}
inline void print_buf(std::ostream& out, const char *title, const std::string &data) {
    return print_buf(out, title, data.data(), data.size());
}
#else
__attribute__((unused)) static void print_buf(FILE *stream, const char *title, const unsigned char *buf, size_t buf_len)
{
    size_t i = 0;
    fprintf(stream, "%s\n", title);
    for(i = 0; i < buf_len; ++i)
    fprintf(stream, "%02X%s", buf[i],
             ( i + 1 ) % 16 == 0 ? "\r\n" : " " );
    fprintf(stream, "\n");
}
#include <time.h>
__attribute__((unused)) static void print_time(FILE *stream) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // struct tm *my_tm = localtime(&ts.tv_sec);
    fprintf(stream, "%lu.%lu\n", ts.tv_sec, ts.tv_nsec);
}
#endif

#endif


