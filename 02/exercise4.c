#include <tm/tmonitor.h>

INT func(INT a)
{
    return a*2;
}

EXPORT INT usermain(void)
{
    INT i = 1;
    i = func(i);
    tm_printf("i=%d\n", i);
    return 0;
}