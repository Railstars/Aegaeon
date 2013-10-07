#ifndef __AVR_PROGMEM_H__
#define __AVR_PROGMEM_H__

#include <stdint.h>

#define PROGMEM 

uint8_t pgm_read_byte(void* ptr);

#endif
