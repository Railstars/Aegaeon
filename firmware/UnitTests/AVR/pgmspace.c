#include <stdint.h>

uint8_t pgm_read_byte(void* ptr)
{
    uint8_t* uiptr = (uint8_t*)ptr;
  return (uint8_t)(*uiptr);
}
