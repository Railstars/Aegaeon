/*
 * File:   DCC_Decoder_Test.cpp
 * Author: dgoodman
 *
 * Created on September 28, 2012, 9:18 AM
 */

extern "C"
{
#include "DCC_Config.h"
#include "DCC_Packet.h"
#include "FX.h"
#include "Motor.h"
#include "DCC_Decoder.h"
#include "reset.h"
#include <AVR/io.h>
#include "globals.h"
void TIM0_OVF_vect(void);
}

#include <CppUTest/TestHarness.h>
#include "HardwareDefs.h"

extern uint8_t eeprom[];
extern uint8_t _defaults[];
extern uint8_t prev_packet_kind; //for keeping track of service mode packets
extern uint8_t preamble_bits, input_state, byte_counter, bit_counter;
extern uint32_t _time_of_last_packet;


#define DCC_PREAMBLE 0
#define DCC_DATA 1


TEST_GROUP(DCCDecoderTests)
{
    DCC_Packet_t p;

    void setup()
    {
        DCC_Packet_Init(&p);
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

TEST(DCCDecoderTests, ObjectCreation)
{
    CHECK_TRUE(&p);
}

TEST(DCCDecoderTests, TestInitialize)
{
    CHECK_EQUAL(0, preamble_bits);
}

TEST(DCCDecoderTests, ProcessOne)
{
    DCC_Decoder_Process_Bit(1);
    CHECK_EQUAL(1, preamble_bits);
}

TEST(DCCDecoderTests, ProcessEarlyZero)
{
    DCC_Decoder_Process_Bit(1);
    DCC_Decoder_Process_Bit(0);
    CHECK_EQUAL(0, input_state);
    CHECK_EQUAL(0, preamble_bits);
}

TEST(DCCDecoderTests, ProcessPreamble)
{
    uint8_t i;
    for (i = 0; i < 14; ++i)
        DCC_Decoder_Process_Bit(1);
    DCC_Decoder_Process_Bit(0);
    CHECK_EQUAL(1, input_state);
}

TEST(DCCDecoderTests, ProcessPacketBailOnFailedByteSeperatator)
{
    uint8_t i;
    for (i = 0; i < 14; ++i)
        DCC_Decoder_Process_Bit(1);
    DCC_Decoder_Process_Bit(0);
    for (i = 0; i < 8; ++i)
        DCC_Decoder_Process_Bit(1); //first byte 0xFF
    DCC_Decoder_Process_Bit(1); //expect a 0 here!

    CHECK_EQUAL(0, input_state); //should switch to preamble state, resetting
    CHECK_EQUAL(9, preamble_bits);
    CHECK_EQUAL(0, DCC_flags&0x01); //shouldn't flag that a packet is ready unless there are at least three bytes.
}

TEST(DCCDecoderTests, ProcessPacketIdlePacket)
{
    uint8_t i;
    for (i = 0; i < 14; ++i)
        DCC_Decoder_Process_Bit(1);
    DCC_Decoder_Process_Bit(0);
    for (i = 0; i < 8; ++i)
        DCC_Decoder_Process_Bit(1); //first byte 0xFF
    DCC_Decoder_Process_Bit(0);
    for (i = 0; i < 8; ++i)
        DCC_Decoder_Process_Bit(0); //first byte 0x00
    DCC_Decoder_Process_Bit(0);
    for (i = 0; i < 8; ++i)
        DCC_Decoder_Process_Bit(1); //first byte 0xFF
    DCC_Decoder_Process_Bit(1);

    CHECK_EQUAL(0, input_state); //should switch to preamble state, resetting
    CHECK_EQUAL(1, preamble_bits); //from stop bit, that counts as a preamble bit
    CHECK_EQUAL(1, DCC_flags&0x01); //SHOULD flag that a packet is ready unless there are at least three bytes.
    CHECK_EQUAL(3, DCC_rx_buffer[ready_buf].size);
}

/*******************/

//Test the state machine function

TEST(DCCDecoderTests, UpdateIdlePacket)
{
    DCC_rx_buffer[buf_sel].data[0] = 0xFF;
    DCC_rx_buffer[buf_sel].data[1] = 0x00;
    DCC_rx_buffer[buf_sel].data[2] = 0xFF;
    DCC_rx_buffer[buf_sel].size = 3;
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);
}

TEST(DCCDecoderTests, UpdateBasicSpeedPacket)
{
    DCC_rx_buffer[buf_sel].size = 3;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x7F; //last step
    DCC_rx_buffer[buf_sel].data[2] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL);
}

TEST(DCCDecoderTests, UpdateBasicSpeedPacketAddressedToOther)
{
    DCC_rx_buffer[buf_sel].size = 3;
    DCC_rx_buffer[buf_sel].data[0] = 0x04;
    DCC_rx_buffer[buf_sel].data[1] = 0x7F; //last step
    DCC_rx_buffer[buf_sel].data[2] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);
}

TEST(DCCDecoderTests, UpdateAdvancedSpeedPacket)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x3F; //advanced speed control step
    DCC_rx_buffer[buf_sel].data[2] = 0xFF; //last step, forward
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL);
}

TEST(DCCDecoderTests, FunctionGroup1_FL_FOR)
{
    DCC_rx_buffer[buf_sel].size = 3;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x90; //FLf on
    DCC_rx_buffer[buf_sel].data[2] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL((1 << PA5), PORTA & ((1 << PA5) | (1 << PA6)));
}

TEST(DCCDecoderTests, FunctionGroup1_FL_REV)
{
    Motor_Set_Speed_By_DCC_Speed_Step_128(-1); //reverse it
    Motor_Update();
    DCC_rx_buffer[buf_sel].size = 3;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x90; //FLf on
    DCC_rx_buffer[buf_sel].data[2] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL((1 << PA6), PORTA & ((1 << PA5) | (1 << PA6)));
}

TEST(DCCDecoderTests, FunctionGroup1_FL_F1)
{
    DCC_Config_Write_CV(CV_OUTPUT_LOCATION_FL1, 0x01); //set F1 to trigger output 1
    DCC_rx_buffer[buf_sel].size = 3;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x81; //F1 on
    DCC_rx_buffer[buf_sel].data[2] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    //now, check that speed and functions are the same
    FX_Update();
    TIM0_OVF_vect();
    CHECK_EQUAL((1 << PA5), PORTA & ((1 << PA5) | (1 << PA6)));
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, OpsModeProgramming_ShortForm_PositiveAccelAdjust)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xF2; //acceleration
    DCC_rx_buffer[buf_sel].data[2] = 0x0F; //acceleration value
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x0F, eeprom[23]);
    CHECK_EQUAL(106, DCC_accel_rate);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, OpsModeProgramming_ShortForm_NegativeAccelAdjust_V1)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xF2; //acceleration
    DCC_rx_buffer[buf_sel].data[2] = 0x8F; //acceleration value
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x8F, eeprom[23]);
    CHECK_EQUAL(0, DCC_accel_rate);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, OpsModeProgramming_ShortForm_NegativeAccelAdjust_V2)
{
    //set up a positive acceleration to test against;
    DCC_Config_Write_CV(CV_ACCELERATION_RATE, 0x0F);
    CHECK_EQUAL(106, DCC_accel_rate);
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xF2; //acceleration
    DCC_rx_buffer[buf_sel].data[2] = 0x8F; //acceleration value
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x8F, eeprom[23]);
    CHECK_EQUAL(0, DCC_accel_rate);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, OpsModeProgramming_ShortForm_PositiveDecelAdjust)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xF3; //deceleration adjust
    DCC_rx_buffer[buf_sel].data[2] = 0x0F; //deceleration adjust value
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x0F, eeprom[24]);
    CHECK_EQUAL(106, DCC_decel_rate);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, OpsModeProgramming_ShortForm_NegativeDecelAdjust_V1)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xF3; //deceleration adjust
    DCC_rx_buffer[buf_sel].data[2] = 0x8F; //deceleration adjust value
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x8F, eeprom[24]);
    CHECK_EQUAL(0, DCC_decel_rate);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, OpsModeProgramming_ShortForm_NegativeDecelAdjust_V2)
{
    //set up a positive acceleration to test against;
    DCC_Config_Write_CV(CV_DECELERATION_RATE, 0x0F);
    CHECK_EQUAL(106, DCC_decel_rate);
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xF3; //deceleration adjust
    DCC_rx_buffer[buf_sel].data[2] = 0x8F; //deceleration adjust value
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x8F, eeprom[24]);
    CHECK_EQUAL(0, DCC_decel_rate);
}

TEST(DCCDecoderTests, OpsModeProgramming_LongForm_ShortAddress)
{
    DCC_rx_buffer[buf_sel].size = 5;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0xEC;
    DCC_rx_buffer[buf_sel].data[2] = 0x00; //short address
    DCC_rx_buffer[buf_sel].data[3] = 0x04;
    DCC_rx_buffer[buf_sel].data[4] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2] ^ DCC_rx_buffer[buf_sel].data[3];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();
    CHECK_EQUAL(0x04, eeprom[1]);
    CHECK_EQUAL(0x04, DCC_address);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, RestrictedSpeedCommand)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x3E; //restricted speed control step
    DCC_rx_buffer[buf_sel].data[2] = 0x0F; //enable restriction, Speed Step 15 of 28
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();

    //now, check that speed and functions are the same
    Motor_Set_Speed_By_DCC_Speed_Step_28(29); //full steam ahead! not!
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(125), MOTOR_PWM_CONTROL);
}

//Not implemented in this decoder.
IGNORE_TEST(DCCDecoderTests, RestrictedSpeedCommand_V2)
{
    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x3E; //restricted speed control step
    DCC_rx_buffer[buf_sel].data[2] = 0x0F; //enable restriction, Speed Step 15 of 28
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();

    //now, check that speed and functions are the same
    Motor_Set_Speed_By_DCC_Speed_Step_28(29); //full steam ahead! not!
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(125), MOTOR_PWM_CONTROL);

    DCC_rx_buffer[buf_sel].size = 4;
    DCC_rx_buffer[buf_sel].data[0] = 0x03;
    DCC_rx_buffer[buf_sel].data[1] = 0x3E; //restricted speed control step
    DCC_rx_buffer[buf_sel].data[2] = 0x8F; //disable restriction, Speed Step 15 of 28
    DCC_rx_buffer[buf_sel].data[3] = DCC_rx_buffer[buf_sel].data[0] ^ DCC_rx_buffer[buf_sel].data[1] ^ DCC_rx_buffer[buf_sel].data[2];
    ready_buf = buf_sel;
    DCC_flags |= DCC_FLAGS_READY;
    DCC_Decoder_Update();

    //now, check that speed and functions are the same
    Motor_Set_Speed_By_DCC_Speed_Step_28(29); //full steam ahead! not!
    Motor_Update();
    //Functions_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(255), MOTOR_PWM_CONTROL);
}

//TODO check too short packets!

//TODO CONSISTING!!