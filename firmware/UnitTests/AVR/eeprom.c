#include <stdint.h>

uint8_t eeprom[512] = {0xFF};


void eeprom_write_byte(const uint8_t *addr, uint8_t val)
{
    uint16_t a = addr;
    eeprom[a] = val;
}

void eeprom_update_byte(const uint8_t *addr, uint8_t val)
{
    eeprom_write_byte(addr, val);
}

uint8_t eeprom_read_byte(const uint8_t *addr)
{
    uint16_t a = addr;
    return eeprom[a];
}
