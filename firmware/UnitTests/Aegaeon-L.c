#include <stdint.h> //GLOBALS!
#include "DCC_Packet.h"
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