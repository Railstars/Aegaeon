/*
 * File:   NMRAMotorDecoder_Test.cpp
 * Author: dgoodman
 *
 * Created on October 23, 2012, 1:18 PM
 */

#include <stdlib.h>

extern "C"
{
#include "DCC_Config.h"
#include "DCC_Packet.h"
#include "FX.h"
#include "Motor.h"
#include "DCC_Decoder.h"
#include <AVR/io.h>
#include <AVR/eeprom.h>
#include "reset.h"
#include "globals.h"
void TIM0_OVF_vect(void);
void TIM1_CAPT_vect(void);
void TIM1_COMPA_vect(void);
}

#include "HardwareDefs.h"

extern uint8_t eeprom[];
extern uint8_t _defaults[];
extern uint8_t reset_state;
extern uint8_t input_state;
extern uint8_t preamble_bits;

#define DCC_PREAMBLE 0
#define DCC_DATA 1

#include <CppUTest/TestHarness.h>

TEST_GROUP(NMRAMotorDecoderTests)
{
    DCC_Packet_t p;

    void setup()
    {
        uint8_t i;
        for (i = 0; i <= 106; ++i)
            eeprom[i] = 0xFF;
        DCC_Packet_Init(&p);
        soft_reset();
        DCC_Config_Initialize();
        //DCC_Decoder_Initialize();
        eeprom[CV_OPS_MODE_BASIC_ACK] = 0x00; //force ops mode ack off
        Motor_Initialize();
        FX_Initialize();
    }

    void teardown()
    {

    }

    void sendPreamble(uint8_t count)
    {
        uint8_t i;
        for (i = 0; i < 14; ++i)
            DCC_Decoder_Process_Bit(1);
    }

    void sendByte(uint8_t byte)
    {
        DCC_Decoder_Process_Bit(0);
        uint8_t i;
        for (i = 0; i < 8; ++i)
        {
            DCC_Decoder_Process_Bit((byte >> (7 - i))&0x01);
        }
    }

    void sendStopBit(void)
    {
        DCC_Decoder_Process_Bit(1);
    }

    void sendBit(uint8_t bit)
    {
        DCC_Decoder_Process_Bit(bit & 0x01);
    }
};

TEST(NMRAMotorDecoderTests, ObjectCreation)
{
    CHECK_TRUE(&p);
}

/****** Tests described in both http://nmra.org/standards/candi/dcctbman.html#test_descriptions and http://nmra.org/standards/candi/pdf/loc-proc.pdf *******/

TEST(NMRAMotorDecoderTests, ProcessPacketIdlePacket)
{
    sendPreamble(14);
    sendByte(0xFF);
    sendByte(0x00);
    sendByte(0xFF);
    sendStopBit();

    CHECK_EQUAL(0, input_state); //should switch to preamble state, resetting
    CHECK_EQUAL(1, preamble_bits);
    CHECK_EQUAL(1, DCC_flags); //SHOULD flag that a packet is ready unless there are at least three bytes.
    CHECK_EQUAL(3, DCC_rx_buffer[ready_buf].size);
}

TEST(NMRAMotorDecoderTests, ProcessPacketResetPacket)
{
    reset_state = 0;
    DCC_flags |= 0x02; //force ops mode for this test. Reset has a different meaning in service mode.

    sendPreamble(14);
    sendByte(0x00);
    sendByte(0x00);
    sendByte(0x00);
    sendStopBit();

    DCC_Decoder_Update();
    CHECK_EQUAL(1, reset_state);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_0)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x60);
    sendByte(0x03 ^ 0x60);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_E)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x61);
    sendByte(0x03 ^ 0x61);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_DEENERGIZED, MOTOR_ENABLED_STATE);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_1)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x62);
    sendByte(0x03 ^ 0x62);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(5), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_2)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x63);
    sendByte(0x03 ^ 0x63);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(23), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_3)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x64);
    sendByte(0x03 ^ 0x64);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(41), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_4)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x65);
    sendByte(0x03 ^ 0x65);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(60), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_5)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x66);
    sendByte(0x03 ^ 0x66);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(78), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_6)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x67);
    sendByte(0x03 ^ 0x67);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(97), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_7)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x68);
    sendByte(0x03 ^ 0x68);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_8)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x69);
    sendByte(0x03 ^ 0x69);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(134), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_9)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6A);
    sendByte(0x03 ^ 0x6A);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(152), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_10)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6B);
    sendByte(0x03 ^ 0x6B);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(170), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_11)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6C);
    sendByte(0x03 ^ 0x6C);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(188), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_12)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6D);
    sendByte(0x03 ^ 0x6D);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(206), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_13)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6E);
    sendByte(0x03 ^ 0x6E);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(224), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ForwardSpeed14_14)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6F);
    sendByte(0x03 ^ 0x6F);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);
}

/*****************************/

TEST(NMRAMotorDecoderTests, ReverseSpeed14_0)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x40);
    sendByte(0x03 ^ 0x40);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_E)
{
    sendPreamble(14);
    sendByte(0x03); //0b00000011
    sendByte(0x41); //0b01000001 //reverse bit set, e-stop
    sendByte(0x03 ^ 0x41); //0b01000010 //CRC
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_DEENERGIZED, MOTOR_ENABLED_STATE);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_1)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x42);
    sendByte(0x03 ^ 0x42);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(5), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_2)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x43);
    sendByte(0x03 ^ 0x43);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(23), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_3)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x44);
    sendByte(0x03 ^ 0x44);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(41), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_4)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x45);
    sendByte(0x03 ^ 0x45);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(60), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_5)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x46);
    sendByte(0x03 ^ 0x46);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(78), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_6)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x47);
    sendByte(0x03 ^ 0x47);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(97), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_7)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x48);
    sendByte(0x03 ^ 0x48);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_8)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x49);
    sendByte(0x03 ^ 0x49);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(134), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_9)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x4A);
    sendByte(0x03 ^ 0x4A);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(152), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_10)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x4B);
    sendByte(0x03 ^ 0x4B);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(170), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_11)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x4C);
    sendByte(0x03 ^ 0x4C);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(188), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_12)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x4D);
    sendByte(0x03 ^ 0x4D);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(206), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_13)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x4E);
    sendByte(0x03 ^ 0x4E);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(224), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, ReverseSpeed14_14)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x4F);
    sendByte(0x03 ^ 0x4F);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);
}

/****** Tests described in http://nmra.org/standards/candi/dcctbman.html#test_descriptions *******/

/** Packet Acceptance Test **/

TEST(NMRAMotorDecoderTests, PacketAcceptanceTest)
{
    //send half-speed command
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x68);
    sendByte(0x03 ^ 0x68);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);

    //send estop
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x61);
    sendByte(0x03 ^ 0x61);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();


    //then 1 second of idle commands.
    //verify motor has stopped
    uint16_t i;
    for (i = 0; i < 1000; ++i)
    {
        sendPreamble(14);
        sendByte(0xFF);
        sendByte(0x00);
        sendByte(0xFF);
        sendStopBit();
        TIM0_OVF_vect();
        DCC_Decoder_Update();
        Motor_Update();
    
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0), MOTOR_PWM_CONTROL);
}

/** Bad Address Test **/

TEST(NMRAMotorDecoderTests, BadAddressTest)
{
    //send half-speed command
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x68);
    sendByte(0x03 ^ 0x68);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);

    //now, start sending etsops to invalid addresses
    uint8_t i;
    for (i = 1; i < 255; ++i)
    {
        if (i != 0x03) //valid address, skip
        {
            //send estop
            sendPreamble(14);
            sendByte(i);
            sendByte(0x61);
            sendByte(0x03 ^ 0x61);
            sendStopBit();
            DCC_Decoder_Update();
            Motor_Update();
        
            CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);
        }
    }

    //then a validly addressed message
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x61);
    sendByte(0x03 ^ 0x61);
    sendStopBit();
    TIM0_OVF_vect();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0), MOTOR_PWM_CONTROL);
}

/** Bad Bit Test **/
TEST(NMRAMotorDecoderTests, BadBitTest)
{
    //send half-speed command
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x68);
    sendByte(0x03 ^ 0x68);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);

    //now, start sending etsops with invalid bits
    uint64_t bitstream = 0xfff01984c5; //an entire estop packet, 40 bits long
    uint8_t i, j;
    const uint8_t len = 40;
    for (i = 0; i < len - 1; ++i) //"to the last bit of the checksum"
    {
        for (j = 0; j < len; ++j)
        {
            if (j != i)
                sendBit((bitstream >> (len - j - 1)) & 0x01);
            else
                sendBit(~(bitstream >> (len - j - 1)) & 0x01);
        }
        DCC_Decoder_Update();
        Motor_Update();
    
        CHECK_EQUAL(MOTOR_PWM_LEVEL(115), MOTOR_PWM_CONTROL);
    }

    //and one last time without corruption
    for (j = 0; j < len; ++j)
    {
        sendBit((bitstream >> (len - j - 1)) & 0x01);
    }
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0), MOTOR_PWM_CONTROL);
}

/*** Truncated Packet Test ****/
//Only works for certain truncations…

IGNORE_TEST(NMRAMotorDecoderTests, TruncatedRecovery)
{
    uint64_t bitstream = 0xfff01984c5; //an entire estop packet, 40 bits long
    uint8_t i, j;
    const uint8_t len = 40;
    for (i = 1; i < len - 1; ++i) //"to the last bit of the checksum"
    {
        //send full reverse packet
        sendPreamble(14);
        sendByte(0x03);
        sendByte(0x4F);
        sendByte(0x03 ^ 0x4F);
        sendStopBit();

        DCC_Decoder_Update();
        Motor_Update();

        CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
        CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);

        for (j = 0; j < len - i; ++j) //send estop, truncated a bit at a time…
        {
            sendBit((bitstream >> (len - j - 1)) & 0x01);
        }
        DCC_Decoder_Update();
        Motor_Update();

        CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
        CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);

        //now send a good full speed packet
        sendPreamble(14);
        sendByte(0x03);
        sendByte(0x6F);
        sendByte(0x03 ^ 0x6F);
        sendStopBit();

        DCC_Decoder_Update();
        Motor_Update();

        CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
        CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);

    }
}

/**** Prior Packet Test ****/
TEST(NMRAMotorDecoderTests, PriorPacketTest)
{
    //basically, send a random packet (not addressed to 03!), make sure can decode next packet correctly.

    uint8_t i;
    for (i = 0; i < 100; ++i)
    {
        //send a random packet
        sendPreamble(14);
        uint8_t r1, r2;
        do
            r1 = rand() % 255;
        while (r1 != 0x03);
        r2 = rand() % 255;
        sendByte(r1);
        sendByte(r2);
        sendByte(r1^r2);
        sendStopBit();


        //now send a good full speed packet
        sendPreamble(14);
        sendByte(0x03);
        sendByte(0x6F);
        sendByte(0x03 ^ 0x6F);
        sendStopBit();

        DCC_Decoder_Update();
        Motor_Update();

        CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
        CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);
    }
}

/****** Tests described in http://nmra.org/standards/candi/pdf/loc-proc.pdf *******/

/*** Address Partition Test ***/
TEST(NMRAMotorDecoderTests, AddressPartitionTest)
{
    //0 address
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0xEC); //ops mode program of base address
    sendByte(0x00); //CV 1
    sendByte(0x00);
    sendByte(0x03^0xEC^0x00^0x00);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(03, DCC_address); //should FAIL

    uint8_t r;
    //check to see what happens when addresses are set inside and outside permitted range
    r = rand() % 127 + 1;
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0xEC); //ops mode program of base address
    sendByte(0x00); //CV 1
    sendByte(r);
    sendByte(0x03^0xEC^0x00^r);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(r, DCC_address); //should SUCCEED

    uint8_t r2 = rand() % 128 + 127;
    sendPreamble(14);
    sendByte(r);
    sendByte(0xEC); //ops mode program of base address
    sendByte(0x01); //CV 1
    sendByte(r2);
    sendByte(r^0xEC^0x01^r2);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(r2, DCC_address); //should FAIL
}

//todo check 14 bit addresses
//todo broadcast commands

/** Decoder Control Packet **/
TEST(NMRAMotorDecoderTests, DecoderControlPacket_SoftReset)
{
    reset_state = 0;
    DCC_flags |= 0x02; //force ops mode for this test. Reset has a different meaning in service mode.

    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(1, reset_state);
}

TEST(NMRAMotorDecoderTests, DecoderControlPacket_HardResetShortAddress)
{
    reset_state = 0;
    DCC_flags |= 0x02; //force ops mode for this test. Reset has a different meaning in service mode.
    //set testCVs to dummy values
    eeprom[29] = 7;
    eeprom[31] = 1;
    eeprom[32] = 1;
    eeprom[19] = 1;
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x01); //hard reset
    sendByte(0x03^0x01);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(1, reset_state);
    CHECK_EQUAL(0x06, eeprom[29]);
    CHECK_EQUAL(0x00, eeprom[31]);
    CHECK_EQUAL(0x00, eeprom[32]);
    CHECK_EQUAL(0x00, eeprom[19]);
}

TEST(NMRAMotorDecoderTests, DecoderControlPacket_HardResetBroadcastAddress)
{
    reset_state = 0;
    DCC_flags |= 0x02; //force ops mode for this test. Reset has a different meaning in service mode.
    //set testCVs to dummy values
    eeprom[29] = 7;
    eeprom[31] = 1;
    eeprom[32] = 1;
    eeprom[19] = 1;
    sendPreamble(14);
    sendByte(0x00);
    sendByte(0x00); //soft reset
    sendByte(0x00^0x00);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(1, reset_state);
}

TEST(NMRAMotorDecoderTests, DecoderControlPacket_HardResetLongAddress)
{
    reset_state = 0;
    DCC_flags |= 0x02; //force ops mode for this test. Reset has a different meaning in service mode.
    //set testCVs to dummy values
    eeprom[29] = 7;
    eeprom[31] = 1;
    eeprom[32] = 1;
    eeprom[19] = 1;
    DCC_address_kind = 1;
    DCC_address = 1034;
    sendPreamble(14);
    sendByte(0xC0 | (DCC_address >> 8));
    sendByte(DCC_address & 0xFF);
    sendByte(0x00); //soft reset
    sendByte((0xC0 | (DCC_address >> 8)) ^ (DCC_address & 0xFF) ^ 0x00);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(1, reset_state);
}

IGNORE_TEST(NMRAMotorDecoderTests, DecoderControlPacket_AdvancedAddressing)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x0B); //set advanced addressing
    sendByte(0x03^0x0B);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x20, DCC_CV29 & 0x20);

    sendPreamble(14);
    sendByte(0x03); //this address is OK, because default value of long address is 0, so decoder should automatically revert to 03/short
    sendByte(0x0A); //unset advanced addressing
    sendByte(0x03^0x0A);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x00, DCC_CV29 & 0x20);

    //to broadcast address
    sendPreamble(14);
    sendByte(0x00);
    sendByte(0x0B); //set advanced addressing
    sendByte(0x00^0x0B);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x20, DCC_CV29 & 0x20);

    sendPreamble(14);
    sendByte(0x00); //this address is OK, because default value of long address is 0, so decoder should automatically revert to 03/short
    sendByte(0x0A); //unset advanced addressing
    sendByte(0x00^0x0A);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x00, DCC_CV29 & 0x20);

    //and again to long address
    DCC_address_kind = 1;
    DCC_address = 921;
    reset_state = 0;
    sendPreamble(14);
    sendByte(0xC0 | (DCC_address >> 8));
    sendByte(DCC_address & 0xFF);
    sendByte(0x0B); //set advanced address
    sendByte((0xC0 | (DCC_address >> 8)) ^ (DCC_address & 0xFF) ^ 0x0B);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x20, DCC_CV29 & 0x20);

    DCC_address_kind = 1;
    DCC_address = 921;
    reset_state = 0;
    sendPreamble(14);
    sendByte(0xC0 | (DCC_address >> 8));
    sendByte(DCC_address & 0xFF);
    sendByte(0x0A); //set advanced address
    sendByte((0xC0 | (DCC_address >> 8)) ^ (DCC_address & 0xFF) ^ 0x0A);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x00, DCC_CV29 & 0x20);
}

TEST(NMRAMotorDecoderTests, ConsistControl)
{
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x12); //consist, make forward
    sendByte(0x66); //enable consist address 66
    sendByte(0x03^0x12^0x66);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x66 | 0x80, DCC_consist_address);

    //test by sending a speed command to 03, then to 66
    //full speed command to 03, should be ignored.
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6F);
    sendByte(0x03 ^ 0x6F);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0), MOTOR_PWM_CONTROL);

    //now send a good full speed packet
    //full speed to consist, should be acted upon
    sendPreamble(14);
    sendByte(0x66);
    sendByte(0x6F);
    sendByte(0x66 ^ 0x6F);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);

    //reset by stopping consist.
    sendPreamble(14);
    sendByte(0x66);
    sendByte(0x60);
    sendByte(0x66 ^ 0x60);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0), MOTOR_PWM_CONTROL);

    //all over again again in reverse facing
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x13); //consist, make forward
    sendByte(0x65); //enable consist address 65
    sendByte(0x03^0x13^0x65);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x65, DCC_consist_address);

    //Now, tell loco to full speed, should be ignored,
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x6F);
    sendByte(0x03 ^ 0x6F);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0), MOTOR_PWM_CONTROL);

    //Now, send a speed command to consist, should result in loco running backwards this time
    sendPreamble(14);
    sendByte(0x65);
    sendByte(0x6F);
    sendByte(0x65 ^ 0x6F);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_REVERSE, MOTOR_DIRECTION);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(242), MOTOR_PWM_CONTROL);
}

//00111111        128 Speed Step Mode
//010DDDDD
//
//011DDDDD        28 Speed Step Mode
//100DDDDD        Function Group 1
//1011DDDD        Function Group 2
//11110010        Short Form Acceleration
//11110011        Short Form Deceleration
//111001AA        Long Form Verify
//111011AA        Long Form Write
//111010AA        Long Form Bit Manipulation
// */

//test CVs 1 7 8 29

//test CVs 1 8 12 13 17/18 19 21 22 23 24 25 29 30 31 32 33-42

//chec RO CVs 8

//check remaining CVs for intent' 2 3 4 5 6 9 10 11 65 66 67-94 95 105 106

/***** RP 9.2.3 tests *****/

//check entry to and exit from service mode

TEST(NMRAMotorDecoderTests, ServiceModeEntry)
{
    //decoder begins in service mode, of course. Send a speed packet to make sure it is in ops mode, then check transition
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x60);
    sendByte(0x03 ^ 0x60);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x02, DCC_flags & DCC_FLAGS_OPS_MODE_MASK); //0 = service mode, 1 = ops mode

    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    DCC_Decoder_Update();

    //now, send a service mode packet
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x00, DCC_flags & DCC_FLAGS_OPS_MODE_MASK); //0 = service mode, 2 = ops mode
}

TEST(NMRAMotorDecoderTests, ServiceModeExitV_1)
{
    //decoder begins in service mode, of course. Nevertheless, let's send a reset packet
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    DCC_Decoder_Update();

    //now, send a service mode packet
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x00, DCC_flags & 0x02); //0 = service mode, 2 = ops mode

    //and now, an ops mode packet. Address it to someone else just for fun
    sendPreamble(14);
    sendByte(0x02);
    sendByte(0x60);
    sendByte(0x02 ^ 0x60);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x02, DCC_flags & 0x02); //0 = service mode, 1 = ops mode
}

TEST(NMRAMotorDecoderTests, ServiceModeExitV_2)
{
    //decoder begins in service mode, of course. Nevertheless, let's send a reset packet
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    DCC_Decoder_Update();

    //now, send a service mode packet
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    DCC_Decoder_Update();
    CHECK_EQUAL(0x00, DCC_flags & 0x02); //0 = service mode, 2 = ops mode

    //and now, we delay for 20 milliseconds
    while (_millis_counter < 20)
    {
        TIM0_OVF_vect();
        DCC_Decoder_Update();
        CHECK_EQUAL(0x00, DCC_flags & 0x02); //20ms, still in service mode
    }
    while (_millis_counter <= 20)
    {
        TIM0_OVF_vect();
    }
    DCC_Decoder_Update();
    CHECK_EQUAL(0x02, DCC_flags & 0x02); //0 = service mode, 2 = ops mode
}

//check direct mode first as it is the simplest to implement

TEST(NMRAMotorDecoderTests, DirectModeSupport)
{
    /*
     * RP9.2.3, line 94:
     * "To determine if a Digital Decoder supports Direct Configuration Variable
     * Addressing, the Universal Command Station/Programmer should perform two
     * bit verifies, one verify for a bit value of '0', one verify for a bit
     * value of '1' to the most significant bit within CV #8 (Manufacturer's ID).
     * Acknowledgement of either bit-verify indicates that the decoder fully
     * supports all modes of Direct Configuration Variable Addressing (Verify
     * Byte, Write Byte, & Bit Manipulation).
     */
    uint8_t i;
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //bit manipulation
    sendByte(0x07); //CV 8
    sendByte(0xE7); //verify that bit 7 is 0
    sendByte(0x78^0x07^0xE7);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //bit manipulation
        sendByte(0x07); //CV 8
        sendByte(0xE7); //verify that bit 7 is 0
        sendByte(0x78^0x07^0xE7);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    uint8_t jog_check = MOTOR_PWM_CONTROL;

        for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //bit manipulation
    sendByte(0x07); //CV 8
    sendByte(0xEF); //verify that bit 7 is 1
    sendByte(0x78^0x07^0xEF);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //bit manipulation
        sendByte(0x07); //CV 8
        sendByte(0xEF); //verify that bit 7 is 1
        sendByte(0x78^0x07^0xEF);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    jog_check |= MOTOR_PWM_CONTROL;

    CHECK_EQUAL(0xFF, jog_check);
}

//successful verify

TEST(NMRAMotorDecoderTests, DirectVerify_V1)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    uint32_t timer = _millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog onset
    while (MOTOR_PWM_CONTROL == MOTOR_PWM_LEVEL(0xFF))
    {
        TIM0_OVF_vect();
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(6, _millis_counter - timer); //6msecs, from t=1 above
}


//failed verify

TEST(NMRAMotorDecoderTests, DirectVerify_V2)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for no jog onset
}

//failed verify, this time because of an invalid sequence of commands

TEST(NMRAMotorDecoderTests, DirectVerify_V3)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x03); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x05); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x06); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x74); //verify byte
    sendByte(0x00); //CV 1
    sendByte(0x07); //contents to verify
    sendByte(0x74^0x00^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for no jog onset
}

//successful write

TEST(NMRAMotorDecoderTests, DirectWrite_V1)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    sendPreamble(20);
    sendByte(0x03);
    sendByte(0x00); //soft reset
    sendByte(0x03^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x00^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x00^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x00^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x00^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x00); //CV 1
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x00^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    uint32_t timer = _millis_counter;

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog onset
    while (MOTOR_PWM_CONTROL == MOTOR_PWM_LEVEL(0xFF))
    {
        TIM0_OVF_vect();
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(6, _millis_counter - timer);
    //and verify the contents of the eeprom!
    CHECK_EQUAL(4, eeprom[1]);
}

//unsuccessful write

TEST(NMRAMotorDecoderTests, DirectWrite_V2)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    sendPreamble(20);
    sendByte(0x00);
    sendByte(0x00); //soft reset
    sendByte(0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    sendPreamble(20);
    sendByte(0x00);
    sendByte(0x00); //soft reset
    sendByte(0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    sendPreamble(20);
    sendByte(0x00);
    sendByte(0x00); //soft reset
    sendByte(0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x07); //CV 8
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x07^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x07); //CV 8
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x07^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x07); //CV 8
    sendByte(0x04); //contents to write
    sendByte(0x7C^0x07^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x07); //CV 8
    sendByte(0x06); //contents to write
    sendByte(0x7C^0x07^0x06);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7C); //write byte
    sendByte(0x07); //CV 8
    sendByte(0x05); //contents to write
    sendByte(0x7C^0x07^0x05);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for jog onset
    CHECK_EQUAL(MANUFACTURER_ID, eeprom[8]);
}

//check Direct Bit Manipulation Mode

TEST(NMRAMotorDecoderTests, DirectBitVerify_V1)
{
    uint8_t i;
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //bit manipulation
    sendByte(0x00); //CV 1
    sendByte(0xE8); //verify that bit 0 is 1
    sendByte(0x78^0x00^0xE8);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //bit manipulation
        sendByte(0x00); //CV 1
        sendByte(0xE8); //verify that bit 0 is 1
        sendByte(0x78^0x00^0xE8);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    uint32_t timer = _millis_counter;
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog onset
    while (MOTOR_PWM_CONTROL == MOTOR_PWM_LEVEL(0xFF))
    {
        TIM0_OVF_vect();
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(6, _millis_counter - timer);
}

//failing verify

TEST(NMRAMotorDecoderTests, DirectBitVerify_V2)
{
    uint8_t i;
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //bit manipulation
    sendByte(0x00); //CV 1
    sendByte(0xE0); //verify that bit 0 is 0
    sendByte(0x78^0x00^0xE0);
    sendStopBit();
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //bit manipulation
        sendByte(0x00); //CV 1
        sendByte(0xE0); //verify that bit 0 is 1
        sendByte(0x78^0x00^0xE0);
        sendStopBit();
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for jog onset
}

TEST(NMRAMotorDecoderTests, DirectBitWrite_V1)
{
    uint8_t i;
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //bit manipulation
    sendByte(0x00); //CV 1
    sendByte(0xF0); //write bit 0 to 0
    sendByte(0x78^0x00^0xF0);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //bit manipulation
        sendByte(0x00); //CV 1
        sendByte(0xF0); //write bit 0 to 0
        sendByte(0x78^0x00^0xF0);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    uint32_t timer = _millis_counter;
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog onset
    while (MOTOR_PWM_CONTROL == MOTOR_PWM_LEVEL(0xFF))
    {
        TIM0_OVF_vect();
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(6, _millis_counter - timer);
    CHECK_EQUAL(2, DCC_address); //make sure write took!
}

//failing verify

TEST(NMRAMotorDecoderTests, DirectBitWrite_V2)
{
    uint8_t i;
    //here is the sequence:
    // 3 or more reset
    // 5 or more identical command packets
    // 1 or more reset packets when ack detected
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //bit manipulation
    sendByte(0x07); //CV 8
    sendByte(0xF0); //write bit 0 is 0
    sendByte(0x78^0x07^0xF0);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //bit manipulation
        sendByte(0x07); //CV 8
        sendByte(0xF0); //write bit 0 is 0
        sendByte(0x78^0x07^0xF0);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for jog onset
    CHECK_EQUAL(MANUFACTURER_ID, eeprom[8]);
}

//check address-only mode

TEST(NMRAMotorDecoderTests, AdressOnlyVerify_V1)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // optional power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x01);
        sendByte(0x7C);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //optional power off
    soft_reset();


    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x70); //verify address
    sendByte(0x03); //address 03
    sendByte(0x70^0x03);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x70); //verify address
        sendByte(0x03); //address 03
        sendByte(0x70^0x03);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog onset
}

TEST(NMRAMotorDecoderTests, AdressOnlyVerify_V2)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // optional power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x01);
        sendByte(0x7C);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //optional power off
    soft_reset();


    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x70); //verify address
    sendByte(0x04); //address 04
    sendByte(0x70^0x04);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x70); //verify address
        sendByte(0x04); //address 04
        sendByte(0x70^0x04);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for jog onset
}

TEST(NMRAMotorDecoderTests, AdressOnlyWrite_V1)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // optional power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x01);
        sendByte(0x7C);

        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //optional power off
    soft_reset();


    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //write address
    sendByte(0x7F); //address 127
    sendByte(0x78^0x7F);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //write address
        sendByte(0x7F); //address 127
        sendByte(0x78^0x7F);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog onset
    CHECK_EQUAL(0x7F, DCC_address);
}

TEST(NMRAMotorDecoderTests, AdressOnlyWrite_V2)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // optional power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    //this one should fail, because 00 is not a permitted address!

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x01);
        sendByte(0x7C);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //optional power off
    soft_reset();


    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the verify packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x78); //write address     //0b01111000
    sendByte(0x00); //address 03        //0b00000000
    sendByte(0x78^0x00);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //only one packet sent, should not be acking!!

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x78); //write address
        sendByte(0x00); //address 03
        sendByte(0x78^0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL); //check for jog onset
    CHECK_EQUAL(3, DCC_address);
}

//check physical regsiters

TEST(NMRAMotorDecoderTests, PhysicalRegsiter_Write_CV2)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // optional power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x01);
        sendByte(0x7C);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //optional power off
    soft_reset();


    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the write packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x79); //write CV2
    sendByte(0xFF); //value 0xFF
    sendByte(0x79^0xFF);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(_defaults[2], eeprom[2]); //only one packet sent, should not be acking!!
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x79); //write CV2
        sendByte(0xFF); //value 0xFF
        sendByte(0x79^0xFF);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(0xFF, eeprom[2]);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //check for jog
}

TEST(NMRAMotorDecoderTests, PhysicalRegsiter_Write_CV110)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // optional power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x01);
        sendByte(0x7C);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        DCC_Decoder_Update();
    }

    //optional power off
    soft_reset();


    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the write packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x7E); //write CV2
    sendByte(0xFF); //value 0xFF
    sendByte(0x7E^0xFF);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(_defaults[7], eeprom[7]); //only one packet sent, should not be acking!!
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x7E); //write CV2
        sendByte(0xFF); //value 0xFF
        sendByte(0x7E^0xFF);
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(_defaults[7], eeprom[7]); //only one packet sent, should not be acking!!
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);
}


//check paged addressing

TEST(NMRAMotorDecoderTests, PhysicalRegsiter_Write_CV66)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // NO power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    //write to CV65, should be page 17, register 1!
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x11); //page 17
        sendByte(0x7D ^ 0x11);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the write packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x79); //write register 1
    sendByte(0xFF); //value 0xFF
    sendByte(0x79^0xFF);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(_defaults[66], eeprom[66]); //only one packet sent, should not be acking!!
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x79); //write CV2
        sendByte(0xFF); //value 0xFF
        sendByte(0x79^0xFF);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(0xFF, eeprom[66]);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, PhysicalRegsiter_Verify_CV66)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // NO power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    //write to CV65, should be page 17, register 1!
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x11); //page 17
        sendByte(0x7D ^ 0x11);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the write packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x71); //verify register 1
    sendByte(_defaults[66]); //value 0xFF
    sendByte(0x71^_defaults[66]);
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x71); //verify register 1
        sendByte(_defaults[66]); //value 0xFF
        sendByte(0x71^_defaults[66]);
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, PhysicalRegsiter_Verify_CV66_Fail)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more page preset packets
    // 6 or more page preset or reset packets
    // NO power off followed by power on
    // 3 or more reset packets
    // 5 or more verifies

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page-preset packets
    //write to CV65, should be page 17, register 1!
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7D);
        sendByte(0x11); //page 17
        sendByte(0x7D ^ 0x11);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the write packets
    sendPreamble(20); //20 bits in service mode!
    sendByte(0x71); //verify register 1
    sendByte(_defaults[66] + 1); //value 0xFF
    sendByte(0x71^(_defaults[66] + 1));
    sendStopBit();
    ++_millis_counter;
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);

    for (i = 0; i < 4; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x71); //verify register 1
        sendByte(_defaults[66] + 1); //value 0xFF
        sendByte(0x71^(_defaults[66] + 1));
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x00), MOTOR_PWM_CONTROL);
}

TEST(NMRAMotorDecoderTests, PagedModeAddressWrite)
{
    //here is the sequence:
    // 3 or more reset
    // 5 or more writes to page register
    // 6 or more reset packets
    // 3 or more reset pakets [sic]
    // NO power off followed by power on
    // 5 or more writes
    // 6 or more identical writes

    uint8_t i;

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //five or more page programming packets
    //write to CV65, should be page 17, register 1!
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20);
        sendByte(0x7C);
        sendByte(0x00); //page 0
        sendByte(0x7C ^ 0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //6 or more reset/pagepreset
    for (i = 0; i < 6; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //3 or more reset
    for (i = 0; i < 3; ++i)
    {
        sendPreamble(20);
        sendByte(0x00);
        sendByte(0x00); //soft reset
        sendByte(0x00);
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
    }

    //now, send the write packets
    for (i = 0; i < 5; ++i)
    {
        sendPreamble(20); //20 bits in service mode!
        sendByte(0x79); //write register 1
        sendByte(_defaults[1] + 1); //value 4
        sendByte(0x79^(_defaults[11] + 1));
        sendStopBit();
        ++_millis_counter;
        DCC_Decoder_Update();
        Motor_Update();
    }
    CHECK_EQUAL(0x04, eeprom[1]);
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL);
}

//RP 9.2.4
TEST(NMRAMotorDecoderTests, DCTransition)
{
    //set speed for 7 of 14, forward
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x68);
    sendByte(0x03 ^ 0x68);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x73), MOTOR_PWM_CONTROL); //should be a moderate speed

    //let 30 milliseconds pass
    while(_millis_counter < 30)
        TIM0_OVF_vect();
    TIM1_COMPA_vect(); //overflow vector, triggers transition

    DCC_Decoder_Update();
    Motor_Update();

    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //should be full speed regardless
    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION); //forward
}

TEST(NMRAMotorDecoderTests, DCCTransition)
{
    //set speed for 7 of 14, forward
    sendPreamble(14);
    sendByte(0x03);
    sendByte(0x68);
    sendByte(0x03 ^ 0x68);
    sendStopBit();

    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x73), MOTOR_PWM_CONTROL); //should be full speed regardless

    //let 30 milliseconds pass
    while(_millis_counter < 30)
        TIM0_OVF_vect();
    TIM1_COMPA_vect(); //overflow vector, triggers transition

    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0xFF), MOTOR_PWM_CONTROL); //should be full speed regardless
    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION); //forward
    //now in DC mode. Let's transition back

    TIM1_CAPT_vect();
    DCC_Decoder_Update();
    Motor_Update();
    CHECK_EQUAL(MOTOR_PWM_LEVEL(0x73), MOTOR_PWM_CONTROL); //should be full speed regardless
    CHECK_EQUAL(MOTOR_FORWARD, MOTOR_DIRECTION); //forward

}

//check Initialization of the DCC system.

//check packet timeout and other error conditions