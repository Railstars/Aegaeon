extern "C"
{
#include "DCC_Config.h"
#include "DCC_Packet.h"
#include "DCC_Decoder.h"
}

extern uint8_t DCC_speed_table[128]; //translates a throttle notch into a voltage level. Default is linear from 0% to 100% voltage (expressed as [0..255]).
extern uint8_t DCC_CV29; //to be accessed through accessor methods
//uint8_t DCC_accel_factor, DCC_decel_factor;
extern uint16_t DCC_address;
extern uint8_t eeprom[512];
extern uint8_t _defaults[];


#include <CppUTest/TestHarness.h>

TEST_GROUP(DCCConfigTests)
{
    DCC_Packet_t p;

    void setup()
    {
        uint8_t i;
        for (i = 0; i <= 108; ++i)
            eeprom[i] = 0xFF;
        DCC_Packet_Init(&p);
        DCC_Config_Initialize();
    }

    void teardown()
    {

    }
};

TEST(DCCConfigTests, ObjectCreation)
{
    CHECK_TRUE(&p);
}

TEST(DCCConfigTests, EEPROMInitialization)
{
    uint16_t i;
    for (i = 0; i <= 106; ++i)
        CHECK_EQUAL(_defaults[i], eeprom[i]);
}

TEST(DCCConfigTests, CheckEEPROMCache)
{
    CHECK_EQUAL(0x06, DCC_CV29);
    CHECK_EQUAL(0x03, DCC_address);
}

TEST(DCCConfigTests, CheckCVWrite)
{
    DCC_Config_Write_CV(CV_PRIMARY_ADDRESS, 0x40);
    CHECK_EQUAL(0x40, DCC_address);
}

TEST(DCCConfigTests, CheckCVWriteNonwriteableCV_CV7)
{
    DCC_Config_Write_CV(CV_MANUFACTURER_VERSION, DECODER_VERSION + 1);
    CHECK_EQUAL(DECODER_VERSION, eeprom[CV_MANUFACTURER_VERSION]);
}

TEST(DCCConfigTests, CheckCVWriteNonwriteableCV_CV8)
{
    DCC_Config_Write_CV(CV_MANUFACTURER_ID, MANUFACTURER_ID + 1);
    CHECK_EQUAL(MANUFACTURER_ID, eeprom[CV_MANUFACTURER_ID]);
}

TEST(DCCConfigTests, CheckCVWriteNonwriteableCV_CV96)
{
    DCC_Config_Write_CV(96, 0xAB);
    CHECK_EQUAL(0x00, eeprom[96]);
}

TEST(DCCConfigTests, CheckCVWriteNonwriteableCV_CV97)
{
    DCC_Config_Write_CV(97, 0xAB);
    CHECK_EQUAL(0x00, eeprom[97]);
}

TEST(DCCConfigTests, CheckCVWriteNonwriteableCV_CV104)
{
    DCC_Config_Write_CV(104, 0xAB);
    CHECK_EQUAL(0x00, eeprom[104]);
}

#ifndef RAILCOM

TEST(DCCConfigTests, CheckCVWriteNonwriteableCV_CV28)
{
    DCC_Config_Write_CV(28, 0xAB);
    CHECK_EQUAL(0x00, eeprom[28]);
}

TEST(DCCConfigTests, CheckCVWriteCV29BiDiBits)
{
    uint8_t old = DCC_CV29;
    DCC_Config_Write_CV(29, DCC_CV29 | CV29_RAILCOM);
    CHECK_EQUAL(old, eeprom[29]);
}
#endif

TEST(DCCConfigTests, CheckCVWriteCV29ReservedBits)
{
    DCC_Config_Write_CV(29, DCC_CV29 | CV29_RESERVED2 | CV29_RESERVED1);
    CHECK_EQUAL(_defaults[29], eeprom[29]);
}

TEST(DCCConfigTests, TestHardReset)
{
    DCC_Config_Write_CV(CV_PRIMARY_ADDRESS, 0x40);
    DCC_Config_Write_CV(CV_MANUFACTURER_ID, 0x08);
    CHECK_EQUAL(_defaults[CV_PRIMARY_ADDRESS], eeprom[CV_PRIMARY_ADDRESS]);
}

TEST(DCCConfigTests, TestSoftReset)
{
    DCC_Config_Write_CV(CV_SPEED_TABLE_1, 0xFF);
    DCC_Config_Write_CV(CV_SPEED_TABLE_28, 0x00);
    DCC_Config_Write_CV(CV_MANUFACTURER_ID, 0x09);
    CHECK_EQUAL(0xFF, eeprom[CV_SPEED_TABLE_1]);
    CHECK_EQUAL(0x00, eeprom[CV_SPEED_TABLE_28]);
}

TEST(DCCConfigTests, TestLongAddressLoad)
{
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 5)); //enable long addresses
    DCC_Config_Write_CV(17, 196);
    DCC_Config_Write_CV(18, 210);
    CHECK_EQUAL(1234, DCC_address);
}

TEST(DCCConfigTests, TestLongAddressLoadOverwriteWithShortAddress)
{
    //From RP9.2.3, line 141
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 5)); //enable long addresses
    DCC_Config_Write_CV(17, 196);
    DCC_Config_Write_CV(18, 210);
    CHECK_EQUAL(1234, DCC_address);

    DCC_Config_Write_CV(1, 3); //write to primary address, should reset everything else
    CHECK_EQUAL(3, DCC_address);
}

TEST(DCCConfigTests, TestPartialLongAddress_RP_921_footnote_9_V1)
{
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 5)); //enable long addresses
    DCC_Config_Write_CV(17, 196);
    //don't write 18!
    //force reset
    DCC_Config_Load_RAM_Cache();
    CHECK_EQUAL(3, DCC_address);
}

TEST(DCCConfigTests, TestPartialLongAddress_RP_921_footnote_9_V2)
{
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 5)); //enable long addresses
    DCC_Config_Write_CV(17, 196);
    DCC_Config_Write_CV(18, 210);
    DCC_Config_Write_CV(17, 000);
    DCC_Config_Load_RAM_Cache();
    CHECK_EQUAL(1234, DCC_address);
}

TEST(DCCConfigTests, TestPartialLongAddress_RP_921_footnote_9_V3)
{
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 5)); //enable long addresses
    DCC_Config_Write_CV(CV_EXTENDED_ADDRESS_MSB, 196);
    DCC_Config_Write_CV(CV_EXTENDED_ADDRESS_LSB, 210);
    DCC_Config_Write_CV(CV_EXTENDED_ADDRESS_MSB, 000);
    DCC_Config_Load_RAM_Cache();
    DCC_Config_Write_CV(18, 210);
    CHECK_EQUAL(210, DCC_address);
}

TEST(DCCConfigTests, TestZeroAddressShort)
{
    DCC_Config_Write_CV(CV_PRIMARY_ADDRESS, 0x04); //get away from default address
    DCC_Config_Write_CV(CV_PRIMARY_ADDRESS, 0x00); //invalid zero address
    CHECK_EQUAL(0x04, DCC_address);
}

TEST(DCCConfigTests, TestZeroAddressLong)
{
    DCC_Config_Write_CV(29, DCC_CV29 | (1 << 5)); //enable long addresses
    DCC_Config_Write_CV(17, 196); //get away from default address
    DCC_Config_Write_CV(18, 210); //get away from default address
    DCC_Config_Write_CV(17, 0x00); //invalid zero address
    DCC_Config_Write_CV(18, 0x00);
    CHECK_EQUAL(1234, DCC_address);
}