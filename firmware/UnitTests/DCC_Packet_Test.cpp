extern "C"
{
#include "DCC_Config.h"
#include "DCC_Packet.h"
}
extern uint16_t DCC_address;

extern uint8_t eeprom[];
extern uint8_t DCC_CV29;
extern uint8_t DCC_flags;

#include <CppUTest/TestHarness.h>

TEST_GROUP(DCCPacketTests)
{
    DCC_Packet_t p;

    void setup()
    {
        DCC_Packet_Init(&p);
        uint16_t i;
        for (i = 0; i < 106; ++i)
            eeprom[i] = 0xFF;
        DCC_flags |= 0x02; //force ops mode
        DCC_Config_Initialize();

    }

    void teardown()
    {

    }
};

TEST(DCCPacketTests, ObjectCreation)
{
    CHECK_TRUE(&p);
}

TEST(DCCPacketTests, ObjectInitializesToIdle)
{
    CHECK_EQUAL(DCC_PACKET_IDLE_KIND, p.kind);
    CHECK_EQUAL(3, p.size);
    CHECK_EQUAL(1, p.data_start);
    CHECK_EQUAL(0xFF, p.data[0]);
    CHECK_EQUAL(0x00, p.data[1]);
    CHECK_EQUAL(0xFF, p.data[2]);
}

IGNORE_TEST(DCCPacketTests, GetKindIsIdle)
{
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_IDLE_KIND, p.kind);
    CHECK_EQUAL(DCC_PACKET_IDLE_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindIsIdleCorrectDataStart)
{
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_IDLE_KIND, p.kind);
    CHECK_EQUAL(1, p.data_start);
}

/*********************/

TEST(DCCPacketTests, GetKindIsReset)
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x00;
    p.data[2] = 0x00;
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SOFT_RESET_KIND, p.kind);
}

/*********************/

TEST(DCCPacketTests, GetKindIsEStopV1)
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x41;
    p.data[2] = p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindIsEStopV2)
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x61;
    p.data[2] = p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindIsEStopV3)
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x71;
    p.data[2] = p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
}

/*********************/

TEST(DCCPacketTests, GetAddressShortV1)
{
    p.size = 3;
    p.data[0] = 0x03; //short address 3
    p.data[1] = 0x00;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(0x00, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(0x03, a);
}

TEST(DCCPacketTests, GetAddressShortV2) //check what happens with broadcast addresses
{
    p.size = 3;
    p.data[0] = 0x00; //broadcast address 0x00
    p.data[1] = 0x00;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(0x00, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(DCC_address, a);
}

TEST(DCCPacketTests, GetAddressAccessory) //should bail on accessory addresses; will use seperate method for that
{
    p.size = 3;
    p.data[0] = 0x83; //accessory address 0x03
    p.data[1] = 0x00;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(0x02, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(0x00, a);
}

TEST(DCCPacketTests, GetAddressLongV1)
{
    p.size = 3;
    p.data[0] = 0xC0; //long address 0x03
    p.data[1] = 0x03;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(0x01, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(0x03, a);
}

TEST(DCCPacketTests, GetAddressLongV2)
{
    p.size = 3;
    p.data[0] = 0xE7; //longest possible long address
    p.data[1] = 0xFF;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(1, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(0x27ff, a);
}

TEST(DCCPacketTests, GetAddressLongV3) //TOO LONG!
{
    p.size = 3;
    p.data[0] = 0xE8; // one longer than longest possible long address
    p.data[1] = 0xFF;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(0x00, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(0x00, a);
}

TEST(DCCPacketTests, GetAddressLongV4) // MUCH TOO LONG!
{
    p.size = 3;
    p.data[0] = 0xFE; //not quite an idle packet
    p.data[1] = 0xFF;
    p.data[2] = 0x00;
    uint16_t a;
    CHECK_EQUAL(0x00, DCC_Packet_Get_Address(&p, &a));
    CHECK_EQUAL(0x00, a);
}

//TODO be done: get accessory packet address

/*********************/

TEST(DCCPacketTests, GetKindBasicSpeedV1)
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x40;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindBasicSpeedV2) //boundary value
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x7F;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindBasicSpeedV3) //speed in reverse
{
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x60;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindAdvancedSpeedV1)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x3F;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[3];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindAdvancedSpeedV2) //check for advanced e_stop
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x3F;
    p.data[2] = 0x01; //really an e-stop!
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[3];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindRestrictedSpeedKind)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x3E;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[3];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_RESTRICTED_SPEED_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindDecoderHardReset_RP921_L121)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x01;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_HARD_RESET_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindFactoryTestInstruction_RP921_L124)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x02;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindDecoderAndConsistControlReservedV1_RP921_L127)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x04;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindSetDecoderFlags_RP921_L128)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x06;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindDecoderAndConsistControlReservedV2_RP921_L129)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x08;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindDecoderSetAdvancedAddressing_RP921_L130)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x0A;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SET_ADVANCED_ADDRESSING_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindDecoderAndConsistControlReservedV3_RP921_L131)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x0C;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindDecoderAckRequest_RP921_L132)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x0E;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ACK_REQUEST_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindFuncGroupOne_RP921_L240)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0x80;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_FUNC_FL_F4_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindFuncGroupTwo_V1_RP921_L251)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xB0;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_FUNC_F5_F8_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindFuncGroupTwo_V2_RP921_L251)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xA0;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_FUNC_F9_F12_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindBinaryStateLong_RP921_L271)
{
    p.size = 5;
    p.data[0] = 0x00;
    p.data[1] = 0xC0;
    p.data[2] = 0x00;
    p.data[3] = 0x00;
    p.data[4] = p.data[0] ^ p.data[1] ^ p.data[2] ^ p.data[3];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_BINARY_STATE_LONG_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindBinaryStateShort_RP921_L286)
{
    p.size = 5;
    p.data[0] = 0x00;
    p.data[1] = 0xDD;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_BINARY_STATE_SHORT_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindFuncGroupThree_RP921_L303)
{
    p.size = 5;
    p.data[0] = 0x00;
    p.data[1] = 0xDE;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_FUNC_F13_F20_KIND, p.kind);
}

IGNORE_TEST(DCCPacketTests, GetKindFuncGroupFour_RP921_L315)
{
    p.size = 5;
    p.data[0] = 0x00;
    p.data[1] = 0xDF;
    p.data[2] = 0x00;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_FUNC_F21_F28_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindFeatureExpansionReserverd_V1_RP921_L326)
{
    p.size = 5;
    p.data[0] = 0x00;
    p.data[1] = 0xC1;
    p.data[2] = 0x00;
    p.data[3] = 0x00;
    p.data[4] = p.data[0] ^ p.data[1] ^ p.data[2] ^ p.data[3];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

TEST(DCCPacketTests, GetKindFeatureExpansionReserverd_V2_RP921_L326)
{
    p.size = 5;
    p.data[0] = 0x00;
    p.data[1] = 0xDC;
    p.data[2] = 0x00;
    p.data[3] = 0x00;
    p.data[4] = p.data[0] ^ p.data[1] ^ p.data[2] ^ p.data[3];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

//IGNORE_TEST(DCCPacketTests, GetKindOpsModeShort_RP921_L338)
//{
//    p.size = 4;
//    p.data[0] = 0x00;
//    p.data[1] = 0xF0;
//    p.data[2] = 0x00;
//    p.data[4] = p.data[0] ^ p.data[1] ^ p.data[2];
//    CHECK_EQUAL(DCC_PACKET_OPS_MODE_SHORT_CONFIG_KIND, p.kind);
//}

TEST(DCCPacketTests, GetKindOpsModeLong_RP921_L353)
{
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xE0;
    p.data[2] = 0x00;
    p.data[4] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_OPS_MODE_LONG_CONFIG_KIND, p.kind);
}

//Can only distinguish Func Group 1 instructions from Accessory Basic Commands by the address!!

IGNORE_TEST(DCCPacketTests, GetKindIgnoreAccessoryBasicPackets_RP921_L402)
{
    //accessory address 0, deactivate device 0
    p.size = 3;
    p.data[0] = 0x80;
    p.data[1] = 0x80;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_NONE_KIND, p.kind);
}

/*********************/

TEST(DCCPacketTests, CheckCRC_V1_RP921_L097)
{
    //start with Idle packet, the default
    CHECK_TRUE(DCC_Packet_Check_CRC(&p));
}

TEST(DCCPacketTests, CheckCRC_V2_RP921_L097)
{
    p.data[2] = 0x88; //bad CRC
    //start with Idle packet, the default
    CHECK_FALSE(DCC_Packet_Check_CRC(&p));
}

TEST(DCCPacketTests, CheckCRC_V3_RP921_L097)
{
    p.size = 6;
    p.data[0] = 0xAB; //random values
    p.data[1] = 0xF3;
    p.data[2] = 0x49;
    p.data[3] = 0x01;
    p.data[4] = 0xFF;
    p.data[5] = p.data[0] ^ p.data[1] ^ p.data[2] ^ p.data[3] ^ p.data[4];
    CHECK_TRUE(DCC_Packet_Check_CRC(&p));
}

TEST(DCCPacketTests, CheckCRC_V4_RP921_L097)
{
    p.size = 6;
    p.data[0] = 0xAB; //random values
    p.data[1] = 0xF3;
    p.data[2] = 0x49;
    p.data[3] = 0x01;
    p.data[4] = 0xFF;
    p.data[5] = (p.data[0] ^ p.data[1] ^ p.data[2] ^ p.data[3] ^ p.data[4]) + 1; // Bad CRC
    CHECK_FALSE(DCC_Packet_Check_CRC(&p));
}

TEST(DCCPacketTests, CheckCRC_V4_Incomplete_packet)
{
    p.size = 2;
    p.data[0] = 0xFF; //random values
    p.data[1] = 0xFF;
    CHECK_FALSE(DCC_Packet_Check_CRC(&p));
}

/*********************/
//extract speed from speed packets, basic and advanced

//Notice that speed checks depend on having a valid kind stored in the packet!!!

TEST(DCCPacketTests, CheckBasicSpeed_V1_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03; //dont want it to look like a reset packet
    p.data[1] = 0x60; //stop, forward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V1_2_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03; //dont want it to look like a reset packet
    p.data[1] = 0x70; //stop, forward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V2_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x40; //stop, reverse
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(-1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V2_2_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x50; //stop, reverse
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(-1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V3_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x61; //e-stop, forward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
    CHECK_EQUAL(1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V3_2_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x71; //e-stop, forward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
    CHECK_EQUAL(1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V4_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x41; //e-stop, backward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
    CHECK_EQUAL(-1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V4_2_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x51; //e-stop, backward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
    CHECK_EQUAL(-1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V5_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x62; //first step
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(2, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V6_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x72; //second step
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(3, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V7_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x7F; //last step
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(29, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V8_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x42; //first step
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(-2, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V9_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x52; //second step
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(-3, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckBasicSpeed_V10_S92_036)
{
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x5F; //last step
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(-29, DCC_Packet_Get_Speed(&p));
}

//ADVANCED SPEED PACKETS!

TEST(DCCPacketTests, CheckAdvancedSpeed_V1_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x80; //stop, forward
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
    CHECK_EQUAL(1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V2_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x00; //stop, reverse
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
    CHECK_EQUAL(-1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V3_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x81; //e-stop, forward
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
    CHECK_EQUAL(1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V4_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x01; //stop, reverse
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_E_STOP_KIND, p.kind);
    CHECK_EQUAL(-1, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V5_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x82; //first step, forward
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
    CHECK_EQUAL(2, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V6_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x02; //first step, reverse
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
    CHECK_EQUAL(-2, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V7_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0xFF; //first step, forward
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
    CHECK_EQUAL(127, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, CheckAdvancedSpeed_V8_RP921_201)
{
    p.size = 4;
    p.data[0] = 0x03;
    p.data[1] = 0x3F; //advanced speed control step
    p.data[2] = 0x7F; //first step, reverse
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_ADVANCED_SPEED_KIND, p.kind);
    CHECK_EQUAL(-127, DCC_Packet_Get_Speed(&p));
}

TEST(DCCPacketTests, GetBasicSpeedCV29Reverse)
{
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 0));
    p.size = 3;
    p.data[0] = 0x03;
    p.data[1] = 0x72; //second step forward
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    CHECK_EQUAL(DCC_PACKET_SPEED_KIND, p.kind);
    CHECK_EQUAL(-3, DCC_Packet_Get_Speed(&p));
}

/*********************/
//Check content of function packets

/***** DCC_Packet_Get_Function not used in this project
TEST(DCCPacketTests, GetContentFuncGroupOne_RP921_L240_NoFuncs)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x80;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(0x000000, f);
}

TEST(DCCPacketTests, GetContentFuncGroupOne_RP921_L240_F0)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x81;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0, f);
}

TEST(DCCPacketTests, GetContentFuncGroupOne_RP921_L240_F0_F1)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x83;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0 | DCC_FUNCTION_F1, f);
}

TEST(DCCPacketTests, GetContentFuncGroupOne_RP921_L240_F0_F2)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x87;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0 | DCC_FUNCTION_F1 | DCC_FUNCTION_F2, f);
}

TEST(DCCPacketTests, GetContentFuncGroupOne_RP921_L240_F0_F3)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x8F;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0 | DCC_FUNCTION_F1 | DCC_FUNCTION_F2 | DCC_FUNCTION_F3, f);
}

TEST(DCCPacketTests, GetContentFuncGroupOne_RP921_L240_F0_F4)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x9F;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0 | DCC_FUNCTION_F1 | DCC_FUNCTION_F2 | DCC_FUNCTION_F3 | DCC_FUNCTION_F4, f);
}

TEST(DCCPacketTests, GetContentFuncGroupTwo_RP921_L251_F5)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0xB1;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F5, f);
}

TEST(DCCPacketTests, GetContentFuncGroupThree_RP921_L303_F13)
{
    uint32_t f = 0x00000000;
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xDE;
    p.data[2] = 0x01;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F13, f);
}

TEST(DCCPacketTests, GetContentFuncGroupThree_RP921_L303_Bad_packet)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0xDE;
    p.data[1] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(0x00000000, f);
}

TEST(DCCPacketTests, GetContentFuncGroupFour_RP921_L315_F20)
{
    uint32_t f = 0x00000000;
    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xDF;
    p.data[2] = 0x01;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F21, f);
}
// Putting it all together for multiple functions

TEST(DCCPacketTests, GetContentMultipleFuncs_V1)
{
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x83;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);

    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xDF;
    p.data[2] = 0x01;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0 | DCC_FUNCTION_F1 | DCC_FUNCTION_F21, f);
}

/**
TEST(DCCPacketTests, GetContentMultipleFuncs_V2) {
    uint32_t f = 0x00000000;
    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x83;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);

    p.size = 4;
    p.data[0] = 0x00;
    p.data[1] = 0xDF;
    p.data[2] = 0x01;
    p.data[3] = p.data[0] ^ p.data[1] ^ p.data[2];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);

    p.size = 3;
    p.data[0] = 0x00;
    p.data[1] = 0x81;
    p.data[2] = p.data[0] ^ p.data[1];
    DCC_Packet_Get_Kind(&p);
    DCC_Packet_Get_Function(&p, &f);
    CHECK_EQUAL(DCC_FUNCTION_F0 | DCC_FUNCTION_F21, f);
}
 */
//TODO Test for packets with multiple commands in them!!