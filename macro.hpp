#ifndef R_MACRO_HPP
#define R_MACRO_HPP

#ifndef MACRO_DECAY
#define MACRO_DECAY(m) (m)
#endif

#ifndef _R_MACRO_ENSTRING
#define _R_MACRO_ENSTRING(_s) #_s
#endif

#ifndef MACRO_TO_CSTR
#define MACRO_TO_CSTR(m) _R_MACRO_ENSTRING(MACRO_DECAY(m))
#endif

#ifndef MACRO_EQL
#define MACRO_EQL(a, b) (MACRO_TO_CSTR(a) == MACRO_TO_CSTR(b))
#endif

#endif