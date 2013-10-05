#ifndef __AVR_INTERRUPT_H__
#define __AVR_INTERRUPT_H__

void cli(void);
void sei(void);
extern uint8_t interrupts;

#  define ISR(vector)            \
    void vector (void)

#endif
