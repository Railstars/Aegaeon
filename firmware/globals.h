/*
 * globals.h
 *
 * Created: 11/5/2012 2:20:50 PM
 *  Author: DGOODMAN
 */


#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "DCC_Packet.h"
#include <avr/eeprom.h>
#include <stdint.h>

//TODO this is a mess. Must we handle it this way? Seems like there must be a better way to handle this.

extern int8_t _goal_speed, _current_speed;
//extern    int8_t _max_speed;
extern uint8_t DCC_CV29;
extern uint8_t DCC_consist_address;
extern uint8_t DCC_address_kind;
extern uint8_t BEMF_cutoff, BEMF_period;
extern int16_t BEMF_Kp, BEMF_Ki;
extern uint16_t BEMF_Kf, BEMF_Kf_consist;
extern uint16_t DCC_address;
extern uint32_t _prev_time;
extern uint32_t _jog_time;
extern uint32_t _kick_start_time;
extern uint32_t DCC_accel_rate;
extern uint32_t DCC_decel_rate;
extern uint32_t accum;
extern uint32_t DCC_forward_trim;
extern uint32_t DCC_reverse_trim;
extern uint8_t FX_Active;
extern uint8_t FX[2];
extern uint8_t FX_Brightness[2];
extern uint8_t FX_Dim_Brightness[2];
extern uint8_t FX_Animation[2];
extern uint8_t FX_Animation_Frame[2];
extern uint32_t FX_Prev_Time[2];
extern uint8_t FX_Period[2];

//extern volatile int8_t  _bemf_speed, _bemf_adjust;
extern volatile uint8_t buf_sel, ready_buf, softcount, DCC_flags, Output_Match[2], Output_Match_Buf[2];
extern volatile uint16_t _micros_rollover;
extern volatile uint32_t _millis_counter, _micros_counter, _prev_bemf_time;
//, _bemf_time, _prev_bemf_time;
extern uint8_t DCC_speed_table[];
extern volatile DCC_Packet_t DCC_rx_buffer[2];

#endif /* GLOBALS_H_ */