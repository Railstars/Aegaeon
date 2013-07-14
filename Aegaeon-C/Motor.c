#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>

#include <stdint.h>
#include "DCC_Config.h"
#include "DCC_Decoder.h"
#include "Motor.h"
#include "globals.h"

#define ENABLE (1<<PB1)
#define FX_SHUNTING (1<<6)

volatile uint8_t sample, sample_ready, min_ADC;
int16_t sum_err;

//Forward is controlled by taking OCRB low, and PWM'ing OCRA
//Reverse is the opposite
//Both HIGH = ERROR!
//Both LOW = coast/stop

void Motor_Initialize(void)
{

    //pre-condition: DCC_Config_Initialize has been called first. Interrupts have been disabled

    //first, compute the accel and decel factors.
    //TODO THIS MAY HAVE TO BE CALLED BY DCC_Config if these CV values change!!
    // (the contents of CV#3*.896)/(number of speed steps in use) = accel factor in seconds/speed_step, a weird unit of measure.

    //do other things to set up the hardware.
    //on ATtiny84A, setup TIMER0 for both motor control PWM on output compare B and a millis timer on compare match overflow

    //set OC0A and OC0B to output; OC0A is on PB2, OC0B is on PA7
    DDRB |= (1 << DDB2);
    DDRA |= (1 << DDA7);

    //set up enable pin too
    DDRB |= (1 << DDB1);
    PORTB |= (1 << PB1); //and set to ENABLE to begin with.

    TCCR0A = (1 << COM0B1) | (1 << WGM00) |  (1 << WGM01); // //Clear OCB0 on up-count clear on down-count, fast PWM
    TCCR0B = (1 << CS00); //use /1 prescaler

    TCNT0 = 0;
    OCR0B = OCR0A = 0; //turn output off.

    TIMSK0 |= (1 << TOIE0); //enable overflow interrupt so we can count micros

    _current_speed = _goal_speed = 1; //forward stop
}

ISR(TIM0_OVF_vect)
{
    _micros_rollover += 32; //at 8MHz, /1 prescaler, it takes 32 microseconds to cycle through
    _micros_counter += 32;
    if (_micros_rollover >= 1000) //update millis
    {
        ++_millis_counter;
        _micros_rollover -= 1000;
    }

    //also handle FX
    //if (softcount++ == 0)
    //{
    //    Output_Match[0] = Output_Match_Buf[0];
    //    Output_Match[1] = Output_Match_Buf[1];
    //    if (Output_Match[0])
    //        PORTA |= (1 << PA5); //only turn on if match is non-zero! Prevents flicker when should be off.
    //    if (Output_Match[1])
    //        PORTA |= (1 << PA6);
    //}

    //if (Output_Match[0] == softcount) //just rolled over, turn output off
    //{
    //    PORTA &= ~(1 << PA5);
    //}
    //if (Output_Match[1] == softcount) //just rolled over, turn output off
    //{
    //    PORTA &= ~(1 << PA6);
    //}
}

uint32_t millis(void)
{
    uint32_t m;

    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        m = _millis_counter;
    }
    return m;
}

uint32_t micros(void)
{
    uint32_t m;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        m = _micros_counter;
    }
    return m;
}


void Motor_EStop(int8_t dir)
{
    //first, kill the h-bridge
    //TCCR0A &= ~( (1 << COM0A1) | (1 << COM0B1) ); //disconnect pins from timer
    //PORTA &= ~(1 << PA7); //force PWM lines LOW, causes coast.
    //PORTB &= ~(1 << PB2);
    
    if (dir >= 0)
        _current_speed = _goal_speed = 1; //immediate forward stop
    else
        _current_speed = _goal_speed = -1; //immediate reverse stop
    //_voltage_level = _voltage_adjust = 0; //make sure we don't re-energize!
}

//in all of the following, 0 = stop; 1 is first movement step.

//void Motor_Set_Speed_By_DCC_Speed_Step_14(int8_t notch)
//{
//	Motor_Set_Speed_By_DCC_Speed_Step_128( (notch*18) >> 1);
//}

//wants a value in the range 0..27

void Motor_Set_Speed_By_DCC_Speed_Step_28(int8_t notch)
{
    //valid range is 1..29, -1..-29
    uint8_t new_notch = notch;
    if (notch < 0) new_notch = -notch;
    else new_notch = notch;

    //if (new_notch <= 1) new_notch = 1;
    //else if (new_notch >= 29) new_notch = 127;
    //else 
    new_notch = ((new_notch - 1) *9) >> 1; //notch-1 * 4.5

    if (notch < 0) //reverse it again
        new_notch = -new_notch;
    Motor_Set_Speed_By_DCC_Speed_Step_128(new_notch);
}

//void Motor_Set_Restriction(uint8_t notch)
//{
//notch comes in as a 28-step, need to convert to 128 step;
//valid range is 1..29, -1..-29
//	uint8_t new_notch = notch;
//	if (new_notch <= 1) new_notch = 1;
//	else if (new_notch >= 29) new_notch = 127;
//	else new_notch = ((new_notch - 1) *9) >> 1; //notch-1 * 4.5

//	_max_speed = new_notch;
//}

//void Motor_Remove_Restriction(void)
//{
//	_max_speed = 0;
//}

void Motor_Set_Speed_By_DCC_Speed_Step_128(int8_t notch)
{
    //valid range is -127..-1, 1..127
    //	//converted internally to -127..-1,0..126
    //if(_max_speed)
    //{
    //	if(-notch < _max_speed) //negative
    //	notch = -_max_speed;
    //	else if(notch > _max_speed)
    //	notch = _max_speed;
    //}

    //	if (notch > 0)
    //	_goal_speed = notch - 1;
    //	else
    //	_goal_speed = notch;
    _goal_speed = notch;
}

uint16_t time_delta_16(uint16_t curtime, uint16_t prevtime)
{
    if (curtime < prevtime) //we have a rollover situation
    {
        return (0xFFFF - prevtime) +curtime;
    }
    else
    {
        return curtime - prevtime;
    }
}

uint32_t time_delta_32(uint32_t curtime, uint32_t prevtime)
{
    if (curtime < prevtime) //we have a rollover situation
    {
        return (0xFFFFFFFF - prevtime) +curtime;
    }
    else
    {
        return curtime - prevtime;
    }
}

void Motor_Update(void)
{
    //check to see if we need to start an AD conversion
    uint8_t abs_speed = _current_speed;
    if(_current_speed < 0) abs_speed = -1*_current_speed;

    uint32_t time = micros();
    uint32_t delta = time_delta_32(time, _prev_time);
    _prev_time = time;

    //first calculate next speed step
    if (_goal_speed != _current_speed) //need to update the current speed!
    {
        //not just if at a stop, but take into account the sign of goal_speed! Don't want a kick-start in reverse!
        if( (1 == abs_speed) &&
        ( ((_current_speed > 0) && (_goal_speed > 0)) || ((_current_speed < 0) && (_goal_speed < 0))) &&
        eeprom_read_byte((const uint8_t *) CV_KICK_START) ) //starting to move from a stop
        {
            _kick_start_time = millis();
        }

        uint32_t factor;
        //Acceleration = movement /away/ from 0. //Deceleration = movement /toward/ 0.
        // SEVERAL CASES!
        // current speed > goal speed > 0 : DECEL
        // current speed > 0 > goal speed : DECEL //have to move towards 0 first
        // 0 > current speed > goal speed : ACCEL

        // goal speed > 0 > current speed : DECEL //have to move towards 0 first
        // goal speed > current speed > 0 : ACCEL
        // 0 > goal speed > current speed : DECEL

        if (_current_speed > _goal_speed)
        {
            if (_current_speed < 0) //was < 0
            factor = DCC_accel_rate;
            else
            factor = DCC_decel_rate;
        }
        else //_goal > current
        {
            if (_current_speed > 0) //was > 0
            factor = DCC_accel_rate;
            else
            factor = DCC_decel_rate;
        }

        if (factor == 0) //instant speed, no momentum
        {
            //move us to 0 or to _goal_speed, whichever is closer
            if ((_current_speed < -1) && (-1 < _goal_speed)) //moving from negative speeds to 0; make speed 0
            {
                _current_speed = 1;
            }
            else if ((_current_speed > 1) && (1 > _goal_speed)) //moving from positrive speeds to -1
            {
                _current_speed = -1;
            }
            else
            {
                _current_speed = _goal_speed;
            }
        }
        else //need to calculate the new current speed
        {
            accum += delta;
            if (accum / factor) //if enough time has passed to jump a speed step
            {
                uint8_t incr = (uint8_t) (accum / factor);
                //accum -= incr; //get it back down to a fractional amount
                accum = 0;
                if (_goal_speed > _current_speed)
                {
                    if ((_current_speed < 0) && ((_current_speed + incr) >= 0))//negative speed
                    {
                        _current_speed = 1;
                    }
                    else if ((_current_speed + incr) >= _goal_speed)
                    {
                        _current_speed = _goal_speed;
                    }
                    else
                    {
                        _current_speed += incr;
                    }
                }
                else
                {
                    if ((_current_speed > 0) && ((_current_speed - incr) <= 0))//negative speed
                    {
                        _current_speed = -1;
                    }
                    else if ((_current_speed - incr) <= _goal_speed)
                    {
                        _current_speed = _goal_speed;
                    }
                    else
                    {
                        _current_speed -= incr;
                    }
                }
            }
        }
    }
    else
    {
        accum = 0; //reset the accumulator
    }

    uint8_t tccr0a = TCCR0A;
    int16_t voltage = 0;
    
    //first, let's update the motor outputs to reflect the current FX setting
    if (_current_speed < 0) //if negative
    {
        //set OC0A
        tccr0a &= ~(1 << COM0B1);
        tccr0a |= (1 << COM0A1);
        voltage = Output_Match_Buf[1];
    }
    else
    {
        //set OC0B
        tccr0a &= ~(1 << COM0A1);
        tccr0a |= (1 << COM0B1);
        voltage = Output_Match_Buf[0];
    }
    
    OCR0B = OCR0A = voltage;
    TCCR0A = tccr0a;
}

void Motor_Jog(void)
{
    if (DCC_SERVICE_MODE || eeprom_read_byte((const uint8_t *) CV_OPS_MODE_BASIC_ACK))
    {
        //increase voltage by a small amount for a brief period.
        _jog_time = millis(); //RP 9.2.3, line 47 = 6ms +/- 1ms;
    }
}
