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

//FORWARD =     PA7 HIGH,   PB1 LOW,    PB2 (OC0A) PWM
//REVERSE =     PA7 LOW,    PB1 HIGH,   PB2 (OC0A) PWM
//BRAKE =        PA7 LOW,    PB1 LOW,    PB2 (OC0A) HIGH
//COAST =       PA7 HIGH,   PB1 HIGH,   PB2 (OC0A) HIGH
//DEAD H-BRIDGE PA7 HIGH,   PB1 HIGH,   PB2 (OC0A) LOW

//consider the algorithm at http://www.advice1.com/reference/pidalgorithmref.html

uint8_t PDFF(int16_t set, int16_t measured)
{
    //    int16_t i_set = set;
    //    int16_t i_measured = measured;
    if (0 == set) //setting a 0 voltage, don't bother.
    {
        //but do measure and set the minimum measured voltage!
        //min_ADC = measured;
        return 0;
    }
    
    //check to see if BEMF is enabled
    
    //check to see if we are above the cutoff speed
    
    int16_t err = set - measured;
    sum_err += err;
    //prevent windup
    if (sum_err > 1024) sum_err = 1024;
    else if (sum_err < -1024) sum_err = 1024;
    
    int16_t iterm;
    if(sum_err < 0)
    {
        iterm = -1 * (BEMF_Ki * sum_err) / 256;
        iterm *= -1;  //TODO WTF is this?
    }
    else
    {
        iterm = (BEMF_Ki * sum_err) / 256;
    }
    
    int16_t fterm;
    if(DCC_consist_address) //if in a consist
    {
        fterm = ((BEMF_Kf_consist * (uint16_t)set) / 256) - measured;
    }
    else
    {
        fterm = ((BEMF_Kf * (uint16_t)set) / 256) - measured;
        
    }
    
    int16_t pterm = (BEMF_Kp * fterm) / 256;
    
    int16_t retval = iterm + pterm + set;
    if(retval < 0) return 0;
    else if(retval > 255) return 255;
    else return retval;
}

void Motor_Initialize(void)
{
    //pre-condition: DCC_Config_Initialize has been called first. Interrupts have been disabled
    
    //first, compute the accel and decel factors.
    //TODO THIS MAY HAVE TO BE CALLED BY DCC_Config if these CV values change!!
    // (the contents of CV#3*.896)/(number of speed steps in use) = accel factor in seconds/speed_step, a weird unit of measure.
    
    //do other things to set up the hardware.
    //on ATtiny84A, setup TIMER0 for both motor control PWM on output compare B and a millis timer on compare match overflow
    
    //set PA7, PB1 and PB2 to output, bring PA7 and PB1 low and PB2 HIGH (= BRAKE)
    DDRB |= (1 << DDB1) | (1 << DDB2);
    PORTB |= (1 << PB2); //set PWM HIGH!
    
    
    //FIXME this is wrong! Should be PB1! Write a test for this
    PORTB &= ~(1 << PB2); //and set both direction bits LOW.
    DDRA |= (1 << DDA7);
    PORTA &= ~(1 << PA7);
    
    
    TCCR0A = (1 << WGM00); // phase correct PWM
    if(eeprom_read_byte((const uint8_t*)CV_MOTOR_FREQUENCY)) // positive value = high frequency, use fast PWM
        TCCR0A |= (1 << WGM01);
    TCCR0B = (1 << CS00); //use /1 prescaler
    
    TCNT0 = 0;
    OCR0A = 0xFF; //turn output off.
    
    TIMSK0 |= (1 << TOIE0); //enable overflow interrupt so we can count micros
    
#ifdef USE_BEMF
    //enable the ADC for BEMF measurement
    DIDR0 |= (1 << ADC3D);
    ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS0); //enable ADC, enable interrupt, set prescalar to 32 to get a clock of 250KHz. See pg 135 of ATtiny84a manual
    ADCSRB |= (1 <<  ADLAR); //left justified ADC output for 8-bit reads
    ADMUX = 0x03; //enable ADC3 for analog conversion, use VCC as reference.
#endif //USE_BEMF
    
    _current_speed = _goal_speed = 1; //forward stop
    min_ADC = 0x93;
}

#ifdef USE_BEMF
ISR(ADC_vect) //ISR for ADC conversion complete
{
    sample = (uint8_t)((1036*(ADCH - min_ADC))>>8);
    _prev_bemf_time = millis(); //set the time of last measurement
    sample_ready = 1;
}
#endif //USE_BEMF

ISR(TIM0_OVF_vect)
{
    _micros_rollover += 32; //at 8MHz, /1 prescaler, it takes 32 microseconds to cycle through
    _micros_counter += 32;
    if (_micros_rollover >= 1000) //update millis
    {
        ++_millis_counter;
        _micros_rollover -= 1000;
    }
    
#ifdef USE_FX
    //also handle FX
    if (softcount++ == 0)
    {
        Output_Match[0] = Output_Match_Buf[0];
        Output_Match[1] = Output_Match_Buf[1];
        if (Output_Match[0])
            PORTA |= (1 << PA5); //only turn on if match is non-zero! Prevents flicker when should be off.
        if (Output_Match[1])
            PORTA |= (1 << PA6);
    }
    
    if (Output_Match[0] == softcount) //just rolled over, turn output off
    {
        PORTA &= ~(1 << PA5);
    }
    if (Output_Match[1] == softcount) //just rolled over, turn output off
    {
        PORTA &= ~(1 << PA6);
    }
#endif //USE_FX
    
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
#ifdef USE_MOTOR
    //NMRA requires that we remove power from motor. We do this by disconnecting PA7 so we can force it low, then bring PB1 and PB2 high.
    //COAST =       PA7 HIGH,   PB1 HIGH,   PB2 (OC0A) HIGH
    //set OC0A first to avoid a shoot-through condition
    TCCR0A &= ~( (1 << COM0A1) ); //disconnect PA7 from timer
    PORTB |= (1 << PB2) | (1 << PB1); //force PWM lines HIGH, turns off motor.
    PORTA |= (1 << PA7); //these two lines may not be necessary. Possibly even dangerous.
#endif //USE_MOTOR
    
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
    if (notch < 0)
        new_notch = -notch;
    else
        new_notch = notch;
    
    if (new_notch > 1) //don't mess with notches 0 or 1, leave those be.
    {
        if (new_notch >= 29) //our rounding algorithm messes up on this case: it sets 29 to 126, which is wrong. This is the expedient fix.
            new_notch = 127;
        else
            new_notch = ((new_notch - 1) *9) >> 1; //notch-1 * 4.5
    }
    
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
    _goal_speed = notch; //will be a value between 1..127, with 1 = stop.
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
	
#ifdef USE_BEMF
    if ((abs_speed < BEMF_cutoff) && (time_delta_32(millis(), _prev_bemf_time) >= BEMF_period))
    {
        //first, set the h-bridge to coast
        TCCR0A &= ~( (1 << COM0A1) ); //disconnect PA7 from timer
        //COAST =       PA7 HIGH,   PB1 HIGH,   PB2 (OC0A) HIGH
        //set OC0A first to avoid a shoot-through condition
        TCCR0A &= ~( (1 << COM0A1) ); //disconnect PA7 from timer
        PORTB |= (1 << PB2) | (1 << PB1); //force PWM lines HIGH, turns off motor.
        PORTA |= (1 << PA7); //these two lines may not be necessary. Possibly even dangerous.
        //start an AD conversion
        ADCSRA |= (1 << ADSC);
    }
#endif //USE_BEMF
    
    
#ifdef USE_BEMF	//hackish
    //first, determine whether we need to do anything
    //we only act if
    // a) BEMF is disabled or not active
    // b) -OR- BEMF is enabled and active, and a new sample is ready
    // (E&A&R) | ~(E&A)
    // ~(E&A) | R
    else if ((abs_speed >= BEMF_cutoff) || sample_ready) //if BEMF is inactive, or there is a sample ready
    {
#endif //USE_BEMF
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
            //smooth out the PID curve
            //if(_bemf_speed)
            //        PID.last_error = _goal_speed - _bemf_speed;
        }
        else
        {
            accum = 0; //reset the accumulator
        }
        
        int16_t voltage = 0;
        
        //NOW we set the motor voltage
        
        //three different possibilities: Jogging the motor (highest priority, because this is communicating with CS), Kick-starting the motor, or setting the motor based on set speed step, either forwards or reverse
        
        if (_jog_time)
        {
            voltage = 0xFF;
            if (time_delta_32(millis(), _jog_time) >= 6) ///RP 9.2.3, line 47 = 6ms +/- 1ms;
            {
                _jog_time = 0;
            }
        }
        else if (_kick_start_time)
        {
            voltage = eeprom_read_byte((const uint8_t *) CV_KICK_START);
            
            if (time_delta_32(_kick_start_time, millis()) >= 6)
            {
                _kick_start_time = 0;
            }
        }
        else //use the commanded speed step to set the motor!
        {
            if (_current_speed < 0) //if negative
            {
                //set REVERSE ENABLE
                PORTB &= ~(1 << PB1); //must bring this pin low first, to avoid shoot-through
                PORTA |=  (1 << PA7);
#ifdef USE_MOTOR_FOR_FX
				voltage = Output_Match_Buf[1];
#else //use motor for motor
                voltage = DCC_speed_table[(_current_speed * -1) - 1]; // + _voltage_adjust;
                if (DCC_reverse_trim)
                {
                    voltage = (voltage * DCC_reverse_trim) >> 7 ; // / 128;
                }
#endif //USE_MOTOR_FOR_FX
            }
            else
            {
                //set FORWARD ENABLE
                PORTA &= ~(1 << PA7); //must bring this pin low first, to avoid shoot-through
                PORTB |=  (1 << PB1);
#ifdef USE_MOTOR_FOR_FX
				voltage = Output_Match_Buf[0];
#else //use motor for motor
                voltage = DCC_speed_table[_current_speed - 1]; // + _voltage_adjust;
                if (DCC_forward_trim)
                {
                    voltage = (voltage * DCC_forward_trim) >> 7; // / 128;
                }
#endif //USE_MOTOR_FOR_FX
            }
            
#ifdef USE_MOTOR
            //If switching mode is enabled, half the voltage
            //TODO fix motor half speed issue!
            if(FX_Active & FX_SHUNTING)
                voltage = (uint8_t)(voltage) >> 1;
#endif //USE_MOTOR

#ifdef USE_BEMF
            //now, if enabled and active, adjust the output voltage with PDFF
            abs_speed = _current_speed;
            if(_current_speed < 0) abs_speed = -1*_current_speed;
            if (abs_speed < BEMF_cutoff) //if BEMF is enabled and active
            {
                voltage = PDFF(voltage, sample);
                sample_ready = 0;
            }
#endif //USE_BEMF
        }
        if (voltage < 0) voltage = 0;
        if (voltage > 255) voltage = 255;
        OCR0A = 0xFF - voltage;
        TCCR0A |= (1 << COM0A1); //force connection of PA7 to timer, in case it was disconnected earlier
		
#ifdef USE_BEMF	//hackish
    }
#endif //USE_BEMF
}

void Motor_Jog(void)
{
    if (DCC_SERVICE_MODE || eeprom_read_byte((const uint8_t *) CV_OPS_MODE_BASIC_ACK))
    {
        //increase voltage by a small amount for a brief period.
        _jog_time = millis(); //RP 9.2.3, line 47 = 6ms +/- 1ms;
    }
}
