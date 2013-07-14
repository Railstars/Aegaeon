#ifndef __RESET_H__
#define __RESET_H__

//from http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_softreset

#include <avr/wdt.h>

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3"))) __attribute__((used));

//#define soft_reset()        while(1); //hang and let watchdog timer kick in.

#define soft_reset()        \
do                          \
{                           \
	wdt_enable(WDTO_15MS);  \
	for(;;)                 \
	{                       \
	}                       \
} while(0)

#endif