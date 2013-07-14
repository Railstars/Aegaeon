#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef	__cplusplus
extern "C" {
    #endif

    #define MOTOR_FORWARD 1
    #define MOTOR_REVERSE 0


    //    typedef struct
    //    {
        //        float integral;
        //        float pgain;
        //        float igain;
        //        float dgain;
        //        uint8_t deadband;
        //        uint8_t last_error;
    //    } PID_t;

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
    uint32_t millis_delta(uint32_t prevtime);
    uint32_t time_delta_32(uint32_t curtime, uint32_t prevtime);

    #ifdef	__cplusplus
}
#endif


#endif //__MOTOR_H__
