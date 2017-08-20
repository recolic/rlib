#ifndef _SRC_FSTR_H
#define _SRC_FSTR_H 1

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
namespace rlib {
    class fstr
    {
    public:
        char *operator()(const char *fmt, ...)
        {
            int n;
            int size = 100;     /* Guess we need no more than 100 bytes */
            char *p, *np;
            va_list ap;

            if ((p = (char *)malloc(size)) == NULL)
                throw std::runtime_error("malloc returns null.");

            while (1) {

                /* Try to print in the allocated space */

                va_start(ap, fmt);
                n = vsnprintf(p, size, fmt, ap);
                va_end(ap);

                /* Check error code */

                if (n < 0)
                    throw std::runtime_error("vsnprintf returns " + std::to_string(n));

                /* If that worked, return the string */

                if (n < size)
                    return p;

                /* Else try again with more space */

                size = n + 1;       /* Precisely what is needed */

                if ((np = (char *)realloc (p, size)) == NULL) {
                    free(p);
                    throw std::runtime_error("make_message realloc failed.");
                } else {
                    p = np;
                }
            }
        } //must free after usage.
    };
}
#endif //SRC_FSTR_H
