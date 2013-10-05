#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
//#include <avr/wdt.h>

#include <stdint.h>
#include "DCC_Hardware.h"
#include "DCC_Decoder.h"
#include "DCC_Config.h"
#include "DCC_Packet.h"
#include "Motor.h"
#include "FX.h"
#include "reset.h"

//~80microseconds to process a packet (so far; only handling motor control)
//30microseconds to process non-intersting packets --- this includes time taken out for interrupts!
//10-40 microseconds to process a bit.
//3-4 ms to load RAM cache. We could optimize this.

//GLOBALS!
int8_t _goal_speed,  _current_speed;
//int8_t _max_speed;
uint8_t DCC_CV29;
uint8_t DCC_consist_address;
uint8_t DCC_address_kind;
uint8_t BEMF_cutoff, BEMF_period;
uint8_t FX_Active;
uint8_t FX[2];
uint8_t FX_Brightness[2];
uint8_t FX_Dim_Brightness[2];
uint8_t FX_Animation[2];
uint8_t FX_Animation_Frame[2];
uint8_t FX_Period[2];


int16_t BEMF_Kp, BEMF_Ki;
uint16_t BEMF_Kf, BEMF_Kf_consist;
uint16_t DCC_address;
uint32_t FX_Prev_Time[2];
uint32_t _prev_time;
uint32_t _jog_time;
uint32_t _kick_start_time;
uint32_t DCC_accel_rate;
uint32_t DCC_decel_rate;
uint32_t accum;
uint32_t DCC_forward_trim;
uint32_t DCC_reverse_trim;

volatile int8_t _bemf_speed;
volatile uint8_t buf_sel, ready_buf, softcount, DCC_flags, Output_Match[2], Output_Match_Buf[2];
volatile uint16_t _micros_rollover;
volatile uint32_t _millis_counter, _micros_counter, _prev_bemf_time;
uint8_t DCC_speed_table[127];
volatile DCC_Packet_t DCC_rx_buffer[2];

//uint16_t free;

///////

#ifndef TESTING
int main(void)
{
#ifndef __DEBUG
    OSCCAL = eeprom_read_byte((const uint8_t*)0x1FF);   //<-- THE PROBLEM IS HERE!?
#endif
    
    //wdt_enable(WDTO_2S); //set a very long timeout.
    
    cli();
    
    //TODO uncertain about the necessity of the following
    //set all unused pins to inputs, enable pullups.
    //at boot time, all pins are set to inputs
    //the pins not used are: PA0, PA1, PA4, PA5 (:M and :C only), PA6 (:M and :C only), PB0, and PB3 (reset)
    PORTA |= ( (1 << PA0) | (1 << PA1) | (1 << PA4) );
    PORTB |= ( (1 << PB0) | (1 << PB3) );
    
    //MCUSR = 0;
    
    
    
    //first thing first, call the setup functions
    DCC_Config_Initialize();
    //wdt_reset();
    DCC_Hardware_Initialize();
    //wdt_reset();
    //DCC_Decoder_Initialize(); //nothing gets done here, everything is already initialized to 0!
    Motor_Initialize();
    //wdt_reset();
#ifdef USE_FX
    FX_Initialize();
#endif
    //wdt_reset();
    //now set up watchdog timer
    //wdt_enable(WDTO_15MS);
    
    sei();
    //wdt_enable(WDTO_15MS);
    
    while(1)
    {
        DCC_Decoder_Update();
        //wdt_reset(); //feed the watchdog
        Motor_Update();
        //wdt_reset();
#ifdef USE_FX
        FX_Update();
#endif
        //wdt_reset(); //feed the watchdog
    }
}
#endif