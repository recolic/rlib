#include <rlib/sys/os.hpp>
#include <rlib/require/gcc>

#include <stdio.h>
#define val(m) #m
#define getname(m) val(m)
#define __CC_NAME__ getname(__COMPILER_ID__)
int main()
{
    printf("cc is %s\n", __CC_NAME__);
    return 0;
}
