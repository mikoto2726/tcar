#include <tk/tkernel.h>
#include <sys/queue.h>
#include <tm/tmonitor.h>


EXPORT INT usermain( void )
{
	tm_printf((UB*)"hello,world \n");

	return 0;
}
