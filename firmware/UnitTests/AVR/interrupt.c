#include <stdint.h>

uint8_t interupts;

void cli(void)
{
	interupts = 0;
}
void sei(void)
{
	interupts = 1;
}
