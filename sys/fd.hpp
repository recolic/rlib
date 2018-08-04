#ifndef RLIB_FD_HPP_
#define RLIB_FD_HPP_

#include <rlib/sys/os.hpp>
#if RLIB_OS_ID == OS_WINDOWS
#include <WinDef.h>
#include <WinNT.h>
using fd = HANDLE;
#else
using fd = int;
#endif

#endif
