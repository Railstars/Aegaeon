#ifndef __AVR_EEPROM_H__
#define __AVR_EEPROM_H__

#include <stdint.h>

void eeprom_write_byte(const uint8_t * addr, uint8_t val);
void eeprom_update_byte(const uint8_t * addr, uint8_t val);

uint8_t eeprom_read_byte(const uint8_t * addr);

#endif
