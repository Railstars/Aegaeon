#ifndef __AVR_IO_H__
#define __AVR_IO_H__

#include <stdint.h>


#define COM0B1  5
#define WGM00   0
#define WGM01   1
#define TOIE0   0
#define DDA7    7
#define DDB1    1
#define DDB2    2
#define PB1     1
#define CS00    0

#define DDA5    5
#define DDA6    6

#define PA0     0
#define PA1     1
#define PA2     2
#define PA3     3
#define PA4     4
#define PA5     5
#define PA6     6
#define PA7     7

#define PB0     0
#define PB1     1
#define PB2     2
#define PB3     3
#define PB4     4
#define PB5     5
#define PB6     6
#define PB7     7

extern uint8_t TCCR0A, TCCR0B, TCNT0, OCR0B, OCR0A, DDRA, DDRB, TIMSK0, PORTA, PORTB, TCNT1;

extern uint8_t TCCR1A, TCCR1B, TIMSK1, DIDR0, ACSR, ADMUX, ADCSRA, ADCSRB, ADCH, ADSC;
extern uint16_t OCR1A, ICR1;

#define ICNC1   7
#define ICES1   6
#define OCIE1A  1


#define CS12    2
#define CS11    1
#define CS10    0


#define ICIE1   0


#define ADC2D   2
#define ADC1D   1

#define ACIE    3
#define ACIC    2
#define ACIS1   1
#define ACIS0   0

#define PORTA5 5
#define PORTA6 6

#define ACBG 6

#define ACO 5

#define ADEN    7
#define ADIE    3
#define ADPS0   0
#define ADPS2   2
#define ADLAR   4
#define ADC3D   3

#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4


//#define TESTING

#endif