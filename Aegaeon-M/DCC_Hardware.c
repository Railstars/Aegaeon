#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "DCC_Decoder.h"
#include "DCC_Config.h"
#include "Motor.h"
#include "FX.h"
#include "globals.h"

//extern uint8_t DCC_CV29;

uint16_t prevtime;
uint16_t times[] = {0xFFFF, 0xFFFF}; //initialize to ones, just because
uint8_t time_select;
uint8_t check;
volatile int8_t _last_known_digital_speed;

uint32_t time_delta(uint32_t curtime, uint32_t prevtime);

void DCC_Hardware_Initialize(void)
{
    //time_select = 0;
    //check = 0;
    //Preconditions* interrupts off.

    /*
     * Nothing interesting to set in TCCR1A
     */
    //TCCR1A = 0;

    /*
     * Setting the OCR (timeout) to 0 allows the full TCNT range for
     * the initial period.
     */
    OCR1A = 30000; //set for 30 seconds

    /*
     * Set the interrupt sense and the prescaler
     */
    TCCR1B = (1 << ICNC1) | (1 << ICES1) | (0 << CS12) | (1 << CS11) | (0 << CS10); //set for noise canceling, initialize to rising edge trigger,  /8 prescalin

    /*
     *	Enable both the Input Capture and the Output Capture interrupts.
     *	The latter is used for timeout (0% and 100%) checkin
     */
    TIMSK1 |= (1 << ICIE1); // | (1 << OCIE1A); //enable input capture enable, and output match enable. Output match is for DC timeout.

    //DDRA |= (1<<DDA1) | (1<<DDA2);
    DIDR0 = (1 << ADC2D) | (1 << ADC1D); //disable digital inputs on AIN1 (negative input to comparator)
    ACSR = (1 << ACBG) | (1 << ACIC) | (1 << ACIS1) | (1 << ACIS0); //use internal bandgap reference instead of AAIN0 as positive input; means we need only connect DCC signal to negative input, sweet!

    //initialize ADC
}

/**
 * TIMER1_COMPA()
 *
 * ICP timer Output Compare ISR.
 *
 * The OC interrupt indicates that some edge didn't arrive as
 * expected. This happens when the duty cycle is either 0% (no
 * pulse at all) or 100% (pulse encompasses the entire period).
 */
ISR(TIM1_COMPA_vect)
{
    //DC mode!
    //zeroth, check CV to see if stop or go
    int8_t new_speed;
    if (CV29_POWER_CONVERSION)
    {
        new_speed = 126;
    }
    else
    {
        new_speed = 1; //stop
    }


    uint8_t DC_FX = eeprom_read_byte((const uint8_t *) CV_ALTERNATE_MODE_FUNCTION_STATUS_FL_F9_F12) & 0x01;
    if (!_last_known_digital_speed) //we need to record current speed
        _last_known_digital_speed = _current_speed;
    if (ACSR & (1 << ACO)) //left is high, reverse
    {
        DC_FX = DC_FX << 1;
        Motor_Set_Speed_By_DCC_Speed_Step_128(-new_speed);
    }
    else
    {
        Motor_Set_Speed_By_DCC_Speed_Step_128(new_speed);
    }
    DC_FX |= (eeprom_read_byte((const uint8_t *) CV_ALTERNATE_MODE_FUNCTION_STATUS_F1_F8) & 0x03) << 2;
    FX_SetFunction(DC_FX, FX_GROUP_1, 0);

    OCR1A = TCNT1 + 1000; //every millisecond, have a look
}


#define IS_ONE(x) ((x >= 52) && (x <= 64)) //between 52 and 64 uS
#define IS_ZERO(x) ((x >= 90) && (x <= 10000)) //between 90 and 10000 uS

ISR(TIM1_CAPT_vect)
{
    //uint8_t tccr1b;
    uint16_t icr;
    icr = ICR1; //capture timestamp
    //tccr1b = TCCR1B;
    //TCCR1B = tccr1b ^ (1 << ICES1);            /* reverse sense so we get called on rising and falling edges */
    TCCR1B ^= (1 << ICES1);

    //delay DC conversion another 30 seconds...
    OCR1A = TCNT1 + 30000; //ok if wraps around!
    if (_last_known_digital_speed) //converting back to DCC
    {
        _goal_speed = _last_known_digital_speed;
        _last_known_digital_speed = 0;
    }
    //calculate new time
    times[time_select] = time_delta_16(icr, prevtime);
    prevtime = icr;
    time_select = (time_select + 1) % 2;

    if (check == time_select)
    {
        //beginning of new pulse. Let:s see what we have and record it
        if (IS_ONE(times[0]))
        {
            int16_t delta = times[0] - times[1];
            if(delta < 0) delta *= -1;
            if (delta <= 6) //less than 6uS difference in first and second half, accept
            {
                DCC_Decoder_Process_Bit(1);
            }
            else if (IS_ZERO(times[1])) //might have front and back halves confused!
            {
                check = (check + 1) % 2;
            }
        }
        else if (IS_ZERO(times[0]))
        {
            if (IS_ZERO(times[1])) //less than 6uS difference in first and second half, accept
            {
                DCC_Decoder_Process_Bit(0);
            }
            else if (IS_ONE(times[1])) //might have front and back halves confused!
            {
                check = (check + 1) % 2;
            }
        }
    }
}
