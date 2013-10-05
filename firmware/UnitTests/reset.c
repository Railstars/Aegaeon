#include <stdint.h>
#include "DCC_Decoder.h"
#include "Motor.h"
#include "FX.h"

uint8_t reset_state;
#include <avr/io.h>
#include "globals.h"

//extern PID_t PID;
extern volatile uint8_t sample;
extern int16_t sum_err;
extern volatile uint32_t _prev_bemf_time;
extern volatile uint8_t _last_known_digital_speed;

extern uint8_t FX_Active;
extern uint8_t FX_Brightness[2];
extern uint8_t FX_Animation[2];
extern uint8_t FX_Animation_Frame[2];
extern uint32_t FX_Prev_Time[2];

void soft_reset(void)
{
    //need to reset ALL variables to 0!!
    //PID.last_error = 0;
    //PID.integral = 0.0;

    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;
    OCR0B = 0;
    OCR0A = 0;
    DDRA = 0;
    DDRB = 0;
    TIMSK0 = 0;
    PORTA = 0;
    PORTB = 0;
    TCNT1 = 0;

    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK1 = 0;
    DIDR0 = 0;
    ACSR = 0;
    ADMUX = 0;
    ADCSRA = 0;
    ADCSRB = 0;
    ADCH = 0;
    ADSC = 0;
    OCR1A = 0;
    ICR1 = 0;


    _goal_speed = 0;
    //     _max_speed = 0;
    DCC_CV29 = 0;
    DCC_consist_address = 0;
    DCC_address_kind = 0;
    DCC_address = 0;
    _prev_time = 0;
    _jog_time = 0;
    _kick_start_time = 0;
    DCC_accel_rate = 0;
    DCC_decel_rate = 0;
    accum = 0;
    DCC_forward_trim = 0;
    DCC_reverse_trim = 0;

    BEMF_cutoff = 0;
    BEMF_period = 0;
    BEMF_Kp = 0;
    BEMF_Ki = 0;
    BEMF_Kf = 0;
    sample = 0;
    sum_err = 0;
    _prev_bemf_time = 0;

    _current_speed = 0;
    //_bemf_speed = 0;
    //_bemf_adjust = 0;
    _last_known_digital_speed = 0;
    buf_sel = 0;
    ready_buf = 0;
    softcount = 0;
    DCC_flags = 0;
    _micros_rollover = 0;
    _millis_counter = 0;
    _micros_counter = 0;
    //_bemf_time = 0;
    //_prev_bemf_time = 0;
    //DCC_speed_table[] = 0;
    //DCC_Packet_t DCC_rx_buffer[2] = 0;
    uint8_t i, j;
    for (i = 0; i < 2; ++i)
    {
        DCC_rx_buffer[i].size = 0;
        DCC_rx_buffer[i].data_start = 0;
        DCC_rx_buffer[i].kind = 0;
        for (j = 0; j < 6; ++j)
            DCC_rx_buffer[i].data[j] = 0;
    }

    FX_Active = 0;
    FX_Brightness[0] = 0;
    FX_Animation[0] = 0;

    FX_Brightness[1] = 0;
    FX_Animation[1] = 0;

    FX_Animation_Frame[0] = 0;
    FX_Animation_Frame[1] = 0;
    FX_Prev_Time[0] = 0;
    FX_Prev_Time[1] = 0;

    //DCC_Decoder_Initialize();
    DCC_Config_Initialize();
    Motor_Initialize();
    FX_Initialize();
    reset_state = 1;
}
