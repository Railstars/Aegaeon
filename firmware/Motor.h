#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __AEGAEON_C
#define MOTOR_IS_FORWARD (TCCR0A & (1 << COM0B1))
#else
#define MOTOR_IS_FORWARD (_current_speed > 0)
#endif

#ifdef	__cplusplus
extern "C" {
#endif

void Motor_Initialize(void);
void Motor_Update(void);

void Motor_EStop(int8_t dir);
//void Motor_Set_Speed_By_DCC_Speed_Step_14(int8_t notch);
void Motor_Set_Speed_By_DCC_Speed_Step_28(int8_t notch);
void Motor_Set_Speed_By_DCC_Speed_Step_128(int8_t notch);
//void Motor_Set_Restriction(uint8_t notch);
//void Motor_Remove_Restriction(void);
void Motor_Jog(void);
uint32_t millis(void);
uint16_t time_delta_16(uint16_t curtime, uint16_t prevtime);
uint32_t time_delta_32(uint32_t curtime, uint32_t prevtime);

#ifdef	__cplusplus
}
#endif


#endif //__MOTOR_H__
