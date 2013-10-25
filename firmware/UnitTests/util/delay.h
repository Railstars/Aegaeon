#ifndef __AVR_DELAY_H__
#define __AVR_DELAY_H__

#include <stdint.h>

extern uint16_t delay;

void _delay_loop_1(uint8_t __count);
void _delay_loop_2(uint16_t __count);

#endif
