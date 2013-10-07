extern "C"
{
#include "DCC_Config.h"
#include "DCC_Packet.h"
#include "DCC_Decoder.h"
#include "FX.h"
#include "Motor.h"
#include "globals.h"
#include <AVR/io.h>
#include "reset.h"
void TIM0_OVF_vect(void);
}
#include <CppUTest/TestHarness.h>
#include "HardwareDefs.h"

extern uint8_t eeprom[];

extern uint8_t FX_Active;
extern uint8_t FX_Brightness[2];
extern uint8_t FX_Animation[2];

TEST_GROUP(FXTests)
{

    void setup()
    {
        uint16_t i;
        for (i = 0; i < 106; ++i)
            eeprom[i] = 0xFF;
        soft_reset();
        //first, force ops mode
        DCC_flags |= DCC_FLAGS_OPS_MODE_MASK;
    }

    void teardown()
    {
    }
};

TEST(FXTests, Initialization)
{
    CHECK_EQUAL(((1 << DDA5) | (1 << DDA6)), DDRA & ((1 << DDA5) | (1 << DDA6)));
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    CHECK_EQUAL(OUTPUT2_OFF, OUTPUT2_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn)
{
    //trigger F0f, which by default is attached to PA5
    //void FX_SetFunction(uint8_t new_fx, uint8_t new_fx_group, uint8_t consisted);
    FX_SetFunction(FX_FL, FX_GROUP_1, false); //); //FX_GROUP_1);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, BasicFXCommandOff)
{
    //reset F0f, which by default is attached to PA5
    FX_SetFunction(FX_FL, FX_GROUP_1, false); //); //FX_GROUP_1);
    FX_Update();
    TIM0_OVF_vect();
    FX_SetFunction(0x00, FX_GROUP_1, false);
    FX_Update();
    uint8_t i;
    for (i = 0; i < 255; ++i) //march through one PWM cycle
        TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    CHECK_EQUAL(OUTPUT2_OFF, OUTPUT2_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_Reverse)
{
    //trigger F0r, which by default is attached to PA6
    //set speed to reverse
    Motor_Set_Speed_By_DCC_Speed_Step_28(-1);
    Motor_Update();
    FX_SetFunction(FX_FL, FX_GROUP_1, false);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT2_ON, OUTPUT2_OUTPUT);
}

TEST(FXTests, BasicFXCommandOff_Reverse)
{
    //reset F0r, which by default is attached to PA6
    FX_SetFunction(FX_FL, FX_GROUP_1, false);
    FX_Update();
    TIM0_OVF_vect();
    FX_SetFunction(0x00, FX_GROUP_1, false);
    FX_Update();
    uint8_t i;
    for (i = 0; i < 255; ++i) //march through one PWM cycle
        TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    CHECK_EQUAL(OUTPUT2_OFF, OUTPUT2_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_Remapped)
{
    //remap output 1 to F1
    DCC_Config_Write_CV(CV_OUTPUT_LOCATION_FL1, 0x01);

    FX_SetFunction(FX_F1, FX_GROUP_1, false);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_50percentdim)
{
    //set brightness of output 1 to 6.3% or 10 out of 255
    float brightness = 0.063 * 255;
    uint8_t intBrightness = (uint8_t)brightness;
    
    DCC_Config_Write_CV(CV_OUTPUT_1_BRIGHTNESS, intBrightness);

    //trigger F0f, which by default is attached to PA5
    FX_SetFunction(FX_FL, FX_GROUP_1, false);
    FX_Update();

    TIM0_OVF_vect();
    //Make sure that the FX has come on
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    
    while (softcount < (intBrightness-1))
    {
        TIM0_OVF_vect();
    }
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);

    while (softcount < 0xFF)
    {
        TIM0_OVF_vect();
    }
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_Rule17_Stop_Dim_V1)
{
    //set brightness of output 1 to 6.3% or 10 out of 255
    float brightness = 0.063 * 255;
    uint8_t intBrightness = (uint8_t)brightness;

    DCC_Config_Write_CV(CV_OUTPUT_1_RULE_17_BRIGHTNESS, intBrightness);
    DCC_Config_Write_CV(CV_OUTPUT_1_FX, 0x05);

    //trigger F0f, which by default is attached to PA5
    FX_SetFunction(FX_FL, FX_GROUP_1, false);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);

    while (softcount < (intBrightness-1))
    {
        TIM0_OVF_vect();
    }
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);

    while (softcount < 0xFF)
    {
        TIM0_OVF_vect();
    }
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_Rule17_Stop_Dim_V2)
{
    DCC_Config_Write_CV(CV_OUTPUT_1_RULE_17_BRIGHTNESS, 0x10);
    DCC_Config_Write_CV(CV_OUTPUT_1_FX, 0x05);

    Motor_Set_Speed_By_DCC_Speed_Step_28(5); //make it move
    Motor_Update();

    //trigger F0f, which by default is attached to PA5
    FX_SetFunction(FX_FL, FX_GROUP_1, false); //FX_GROUP_1);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    
    while (softcount < (0x10 - 1))
        TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);

    while (softcount < 0xFF)
        TIM0_OVF_vect();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT); //off for just a moment.
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_Rule17_Moving_Dim_V1)
{
    DCC_Config_Write_CV(CV_OUTPUT_1_RULE_17_BRIGHTNESS, 0x10);
    DCC_Config_Write_CV(CV_OUTPUT_1_FX, 0x11);

    //trigger F0f, which by default is attached to PA5
    FX_SetFunction(FX_FL, FX_GROUP_1, false); //FX_GROUP_1);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    
    while (softcount < (0x10 - 1))
        TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);

    while (softcount < 0xFF)
        TIM0_OVF_vect();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, BasicFXCommandOn_Rule17_Moving_Dim_V2)
{
    DCC_Config_Write_CV(CV_OUTPUT_1_RULE_17_BRIGHTNESS, 0x10);
    DCC_Config_Write_CV(CV_OUTPUT_1_FX, 0x11);

    Motor_Set_Speed_By_DCC_Speed_Step_28(5); //make it move
    Motor_Update();

    //trigger F0f, which by default is attached to PA5
    FX_SetFunction(FX_FL, FX_GROUP_1, false); //FX_GROUP_1);
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    
    while (softcount < (0x10 - 1))
        TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);

    while (softcount < 0xFF)
        TIM0_OVF_vect();
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_OFF, OUTPUT1_OUTPUT);
    TIM0_OVF_vect();
    CHECK_EQUAL(OUTPUT1_ON, OUTPUT1_OUTPUT);
}

TEST(FXTests, AnimatedFXCommandOn_Strobe)
{
    DCC_Config_Write_CV(CV_OUTPUT_1_FX, 0x41); // on forward, animation = strobe

    FX_SetFunction(FX_FL, FX_GROUP_1, false);
    FX_Update();
    CHECK_EQUAL(1, FX_Animation[0]);
}

//TODO add FX tests for consisted!