#include "reset.h"

void wdt_init(void)
{
	MCUSR = 0;
	wdt_disable();

	return;
}
