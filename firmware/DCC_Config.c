#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "DCC_Config.h"
#include "DCC_Decoder.h"
#include "Motor.h"
#include "FX.h"
#include "globals.h"

uint8_t const PROGMEM _defaults[] = {
    0x00, // internal flag byte, allows CV1 to be at address 01
    0x03, // PRIMARY_ADDRESS			1	//mandatory,	3,	RW,	Uniform
    0x01, // V_START					2	//recommended,	x,	RW
    0x00, // ACCELERATION_RATE		3	//recommended,	x,	RW
    0x00, // DECELERATION_RATE		4	//recommended,	x,	RW
    0x00, // V_HIGH					5	//optional,		x,	RW
    0x00, // V_MID					6	//optional,		x,	RW
    DECODER_VERSION, // MANUFACTURER_VERSION	7	//mandatory,	x,	RO
    MANUFACTURER_ID, // MANUFACTURER_ID			8	//mandatory,	x,	RO, Uniform
    0x00, // TOTAL_PWM_PERIOD		9	//optional,		x,	RW
    0x00, // EMF_FEEDBACK_CUTOUT		10	//required,		x,	RW
    
    0x00, // PACKET_TIME_OUT_VALUE	11	//optional,		x,	RW
    0x02, // POWER_SOURCE_CONVERSION	12	//optional,		x,	RW, Uniform
    0x00, // ALTERNATE_MODE_FUNCTION_STATUS_F1_F8		13	//optional,		x,	RW, Uniform
    0x03, // ALTERNATE_MODE_FUNCTION_STATUS_FL_F9_F12		14	//optional,		x,	RW, Uniform
    0x00, // DECODER_LOCK_1			15	//optional,		x,	RW, Uniform
    0x00, // DECODER_LOCK_2			16	//optional,		x,	RW, Uniform
    0x00, // EXTENDED_ADDRESS_MSB		17	//optional,		x,	RW, Uniform
    0x00, // EXTENDED_ADDRESS_LSB		18	//optional,		x,	RW, Uniform
    0x00, // CONSIST_ADDRESS			19	//optional,		x,	RW, Uniform
    0x00, // RESERVED				20	//RESERVED
    
    0x00, // CONSIST_ADDRESS_ACTIVE_FOR_F1_F8		21	//optional,		x,	RW, Uniform
    0x00, // CONSIST_ADDRESS_ACTIVE_FOR_FL_F9_F12		22	//optional,		x,	RW, Uniform
    0x00, // ACCELERATION_ADJUSTMENT	23	//optional,		x,	RW, Uniform
    0x00, // DECELERATION_ADJUSTMENT	24	//optional,		x,	RW, Uniform
    0x00, // SPEED_TABLE/MIDRANGE_SPEED_STEP				25	//optional,		x,	RW, Uniform
    0x00, // RESERVED				26	//RESERVED
    0x00, // DECODER_AUTOMATIC_STOPPING_CONFIG		27	//optional,		x,	RW, Uniform
    0x00, // BI_DIRECTIONAL_COMMUNICIMATION_CONFIG		28	//optional,		x,	RW, Uniform
    0x06, // CONFIGURATION_DATA		29	//mandatory*,	x,	RW, Uniform
    0x00, // ERROR_INFORMATION		30	//optional,		x,	RW, Uniform
    
    0x00, // INDEX_HIGH_BYTE			31	//optional,		x,	RW, Uniform
    0x00, // INDEX_LOW_BYTE			32	//optional,		x,	RW, Uniform
    0x00, // OUTPUT_LOCATION_FLF		33	//optional,		x,	RW, Uniform
    0x03, // OUTPUT_LOCATION_FLR		34	//optional,		x,	RW, Uniform
    0x04, // OUTPUT_LOCATION_FL1		35	//optional,		x,	RW, Uniform
    0x08, // OUTPUT_LOCATION_FL2		36	//optional,		x,	RW, Uniform
    0x10, // OUTPUT_LOCATION_FL3		37	//optional,		x,	RW, Uniform
    0x04, // OUTPUT_LOCATION_FL4		38	//optional,		x,	RW, Uniform
    0x08, // OUTPUT_LOCATION_FL5		39	//optional,		x,	RW, Uniform
    0x10, // OUTPUT_LOCATION_FL6		40	//optional,		x,	RW, Uniform
    
    0x20, // OUTPUT_LOCATION_FL7		41	//optional,		x,	RW, Uniform
    0x40, // OUTPUT_LOCATION_FL8		42	//optional,		x,	RW, Uniform
    0x00, // OUTPUT_LOCATION_FL9		43	//optional,		x,	RW, Uniform
    0x00, // OUTPUT_LOCATION_FL10	44	//optional,		x,	RW, Uniform
    0x00, // OUTPUT_LOCATION_FL11	45	//optional,		x,	RW, Uniform
    0x00, // OUTPUT_LOCATION_FL12	46	//optional,		x,	RW, Uniform
    
    0x01, // CV_OPS_MODE_BASIC_ACK	47	//optional,		x,	RW
    
    0x01, // CV_OUTPUT_1_DIRECTION_RULE17                48
    0xFF, // CV_OUTPUT_1_BRIGHTNESS         49	//optional,		x,	RW
    0xFF, // CV_OUTPUT_1_RULE_17_BRIGHTNESS	50	//optional,		x,	RW
    0x00, // CV_OUTPUT_1_FX             	51	//optional,		x,	RW
    0x3E, // CV_OUTPUT_1_PERIOD 			52, default 2 seconds
    
    0x02, // CV_OUTPUT_2_DIRECTION_RULE17   53
    0xFF, // CV_OUTPUT_2_BRIGHTNESS     	54	//optional,		x,	RW
    0xFF, // CV_OUTPUT_2_RULE_17_BRIGHTNESS	default 0.2     55	//optional,		x,	RW
    0x00, // CV_OUTPUT_2_FX  				56	//optional,		x,	RW
    0x3E, // CV_OUTPUT_1_PERIOD				57, default 2 seconds (62ms)
    
    FIRMWARE_REVISION, // CV_FIRMWARE_REVISION			58
    0x01, // CV_MOTOR_FREQUENCY			59
    
    0x64, // CV_BEMF_PERIOD, default 100ms	60	//optional,		x,	RW
    0x32, // CV_BEMF_PGAIN, default 40%	61	//optional,		x,	RW
    0x18, // CV_BEMF_IGAIN, default 10%  62	//optional,		x,	RW
    0xFF, // CV_BEMF_FGAIN, default 100%	63	//optional,		x,	RW
    0x98, // CV_BEMF_FGAIN_CONSIST, default 60% 64	//optional,		x,	RW
    0x00, // KICK_START				65	//optional,		x,	RW
    0x00, // FORWARD_TRIM			66	//optional,		x,	RW
    0x01, // SPEED_TABLE_1			67	//optional,		x,	RW
    0x0A, // SPEED_TABLE_2			68	//optional,		x,	RW
    0x13, // SPEED_TABLE_3			69	//optional,		x,	RW
    0x1D, // SPEED_TABLE_4			70	//optional,		x,	RW
    
    0x26, // SPEED_TABLE_5			71	//optional,		x,	RW
    0x30, // SPEED_TABLE_6			72	//optional,		x,	RW
    0x39, // SPEED_TABLE_7			73	//optional,		x,	RW
    0x42, // SPEED_TABLE_8			74	//optional,		x,	RW
    0x4C, // SPEED_TABLE_9			75	//optional,		x,	RW
    0x55, // SPEED_TABLE_10			76	//optional,		x,	RW
    0x5F, // SPEED_TABLE_11			77	//optional,		x,	RW
    0x68, // SPEED_TABLE_12			78	//optional,		x,	RW
    0x71, // SPEED_TABLE_13			79	//optional,		x,	RW
    0x7B, // SPEED_TABLE_14			80	//optional,		x,	RW
    
    0x84, // SPEED_TABLE_15			81	//optional,		x,	RW
    0x8E, // SPEED_TABLE_16			82	//optional,		x,	RW
    0x97, // SPEED_TABLE_17			83	//optional,		x,	RW
    0xA0, // SPEED_TABLE_18			84	//optional,		x,	RW
    0xAA, // SPEED_TABLE_19			85	//optional,		x,	RW
    0xB3, // SPEED_TABLE_20			86	//optional,		x,	RW
    0xBD, // SPEED_TABLE_21			87	//optional,		x,	RW
    0xC6, // SPEED_TABLE_22			88	//optional,		x,	RW
    0xCF, // SPEED_TABLE_23			89	//optional,		x,	RW
    0xD9, // SPEED_TABLE_24			90	//optional,		x,	RW
    
    0xE2, // SPEED_TABLE_25			91	//optional,		x,	RW
    0xEC, // SPEED_TABLE_26			92	//optional,		x,	RW
    0xF5, // SPEED_TABLE_27			93	//optional,		x,	RW
    0xFF, // SPEED_TABLE_28			94	//optional,		x,	RW
    0x00, // REVERSE_TRIM			95	//optional,		x,	RW
    0x00, // RESERVED				96	//RESERVED
    0x00, // RESERVED				97	//RESERVED
    0x00, // RESERVED				98	//RESERVED
    0x00, // RESERVED				99	//RESERVED
    0x00, // RESERVED				100	//RESERVED
    
    0x00, // RESERVED				101	//RESERVED
    0x00, // RESERVED				102	//RESERVED
    0x00, // RESERVED				103	//RESERVED
    0x00, // RESERVED				104	//RESERVED
    0x00, // USER_ID_1				105	//optional,		x,	RW
    0x00 // USER_ID_2				106	//optional,		x,	RW
};

uint8_t const PROGMEM _animations[] = {
    /////Strobe
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x24,
    0xFF,
    0x10,
    0x00,
    
    /////Double Stobe
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x24,
    0xFF,
    0x10,
    0x00,
    0x00,
    0x00,
    0x24,
    0xFF,
    0x10,
    0x00,
    
    /////Rotary Beacon
	0x98,
	0xB0,
	0xC6,
	0xD9,
	0xE9,
	0xF5,
	0xFC,
	0xFF,
	0xFC,
	0xF5,
	0xE9,
	0xD9,
	0xC6,
	0xB0,
	0x98,
	0x7F,
	0x66,
	0x4E,
	0x38,
	0x25,
	0x15,
	0x09,
	0x02,
	0x00,
	0x02,
	0x09,
	0x15,
	0x25,
	0x38,
	0x4E,
	0x66,
	0x7F,
    
    /////Firebox
    0xE8,
    0xA5,
    0xF7,
    0x14,
    0xD6,
    0x14,
    0x71,
	0xBF,
    0xB9,
    0x68,
    0xD3,
    0x8F,
    0x7C,
    0x45,
    0x4E,
    0x35,
    0xAD,
    0x22,
    0xC5,
    0x2A,
    0x67,
    0x13,
    0x60,
    0x14,
    0x35,
    0x25,
    0x3E,
    0x9C,
    0x38,
    0x9E,
    0xB1,
    0x6E,
    
    /////Left Ditch
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7F,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xBF,
    0x5F,
    0x2E,
    0x17,
    0x0B,
    0x05,
    0x00,
    0x00,
    0x00,
    0x00,
    
    /////Right Ditch
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xBF,
    0x5F,
    0x2E,
    0x17,
    0x0B,
    0x05,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7F,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    
    /////Mars
    0x20,
    0x20,
    0x20,
    0x2D,
    0x40,
    0x40,
    0x40,
    0x40,
    0x4B,
    0x80,
    0xFF,
    0xFF,
    0xBF,
    0x80,
    0x64,
    0x50,
    0x50,
    0x64,
    0x80,
    0xBF,
    0xFF,
    0xFF,
    0x80,
    0x4B,
    0x40,
    0x40,
    0x40,
    0x40,
    0x2D,
    0x20,
    0x20,
    0x20,
    
    /////Gyra
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x40,
    0x40,
    0x40,
    0x40,
    0x4B,
    0x64,
    0x80,
    0xFF,
    0xFF,
    0x80,
    0x64,
    0x4B,
    0x40,
    0x40,
    0x40,
    0x40,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20
};

//RP 9.2.1, line 121-ff

void DCC_Config_Hard_Reset(void)
{
    eeprom_update_byte((uint8_t *) CV_CONFIGURATION_DATA, _defaults[CV_CONFIGURATION_DATA]);
    eeprom_update_byte((uint8_t *) 31, 0x00);
    eeprom_update_byte((uint8_t *) 32, 0x00);
    eeprom_update_byte((uint8_t *) 19, 0x00);
}

static void _initialize_eeprom(void)
{
    uint16_t i;
    for (i = 0; i < CV_SPEED_TABLE_1; ++i) //skip speed table
    {
        eeprom_update_byte((uint8_t *) i, pgm_read_byte(&_defaults[i])); //((const uint8_t *) (i + CV_DEFAULT_OFFSET)));
    }
    for (i = CV_SPEED_TABLE_28 + 1; i <= CV_USER_ID_2; ++i) //skip speed table
    {
        eeprom_update_byte((uint8_t *) i, pgm_read_byte(&_defaults[i])); //eeprom_read_byte((const uint8_t *) (i + CV_DEFAULT_OFFSET)));
    }
    eeprom_update_byte((uint8_t*) CV_EXTENDED_ADDRESS_MSB_TEMP, 0x00);
    eeprom_update_byte((uint8_t*) CV_PAGE, 0x01);
    
    //now, initialize FX animations
    for (i = 0; i < (FX_NUM_ANIMATIONS*0x20); ++i)
    {
        eeprom_update_byte((uint8_t*) (i+0x70), pgm_read_byte(&_animations[i]));
    }
}

static void _load_address(void)
{
    //first, check to see if we are part of a consist
    DCC_consist_address = eeprom_read_byte((const uint8_t *) CV_CONSIST_ADDRESS);
    
    if (CV29_LONG_ADDRESSING)
    {
        DCC_address = ((eeprom_read_byte((const uint8_t *) CV_EXTENDED_ADDRESS_MSB) & 0x3F) << 8) | eeprom_read_byte((const uint8_t *) CV_EXTENDED_ADDRESS_LSB);
        DCC_address_kind = DCC_LONG_ADDRESS;
        if (DCC_address != 0x0000) //this should not happen, but hey!
        {
            return;
        }
    }
    
    //made it here, so we must need the short address instead
    DCC_address = eeprom_read_byte((const uint8_t *) CV_PRIMARY_ADDRESS);
    DCC_address_kind = DCC_SHORT_ADDRESS;
    if (DCC_address == 0x00)
        DCC_address = 0x03;
}

static void _load_speed_table(void)
{
    //uint8_t adjust;
    //first step, calculate acceleration and deceleration
    DCC_accel_rate = (uint32_t)eeprom_read_byte((const uint8_t *) CV_ACCELERATION_RATE) * 7111; //7111.1111111111113; // = .896 / 126 * 1000 * 1000;
    //check CV 23
    /*
     adjust = eeprom_read_byte((const uint8_t *)CV_ACCELERATION_ADJUSTMENT);
     if (adjust & 0x80) //top bit set means subtract
     {
     DCC_accel_rate -= (adjust & 0x7F) * 7111.1111111111113;
     if (DCC_accel_rate <= 0) DCC_accel_rate = 0;
     }
     else
     DCC_accel_rate += (adjust & 0x7F) * 7111.1111111111113;
     */
    DCC_decel_rate = (uint32_t)eeprom_read_byte((const uint8_t *) CV_DECELERATION_RATE) * 7111; //7111.1111111111113; // = .896 / 126 * 1000 * 1000;
    //check CV 23
    /*
     adjust = eeprom_read_byte((const uint8_t *)CV_DECELERATION_ADJUSTMENT);
     if (adjust & 0x80) //top bit set means subtract
     {
     DCC_decel_rate -= (adjust & 0x7F) * 7111.1111111111113;
     if (DCC_decel_rate <= 0) DCC_decel_rate = 0;
     }
     else
     DCC_decel_rate += (adjust & 0x7F) * 7111.1111111111113;
     */
    DCC_forward_trim = eeprom_read_byte((const uint8_t *) CV_FORWARD_TRIM);
    //    if (DCC_forward_trim)
    //        DCC_forward_trim = DCC_forward_trim * 0.0078125;
    DCC_reverse_trim = eeprom_read_byte((const uint8_t *) CV_REVERSE_TRIM);
    //    if (DCC_reverse_trim)
    //        DCC_reverse_trim = DCC_reverse_trim * 0.0078125;
    
    //next step: compute the speed table
    //two possible sources for the table, 3-step table and 28-step table. Either way, need to fit into the 128 steps we use internally
    
    //INDEX 0 = STOP
    //INDEX 1 = FIRST
    //INDEX 126 = LAST
    if (CV29_FANCY_SPEED_CURVE)
    {
        uint8_t i;
        uint32_t speed;
        uint32_t next_speed;
        uint32_t factor;
        uint8_t table_index;
        //DCC_speed_table[0] = 0; //always!
        for (i = 1; i < 127; ++i)
        {
            //yuck, we have to do division!? Thank god we only do this once.
            //factor = i * 0.21428571428571427; //this is the magic number that scales 1..28 to 1..126
            factor = (i * 384) / 14;
            table_index = factor / 128;
            speed = eeprom_read_byte((const uint8_t *) (table_index + CV_SPEED_TABLE_1));
            if (table_index < CV_SPEED_TABLE_28)
                next_speed = eeprom_read_byte((const uint8_t *) (table_index + CV_SPEED_TABLE_1 + 1));
            else
                next_speed = speed + 9; //9 is a good fudge factor, and represents a reasonable approximation of what the next speed would be in a linear table
            //filter off the top part of factor to get just the fractional part
            factor = factor - (table_index * 128);
            factor = speed + ((next_speed - speed) * factor) / 128; //add a bit to the speed determined by the interval and the fractional part
            DCC_speed_table[i] = (uint8_t) factor; //round it down.
        }
    }
    else
    {
        //TODO check whether these values need to be recalculated!
        //to be interpolated from the start, mid, and high voltage levels
        uint8_t i;
        uint8_t start, mid, high;
        start = eeprom_read_byte((const uint8_t *) CV_V_START);
        high = eeprom_read_byte((const uint8_t *) CV_V_HIGH);
        mid = eeprom_read_byte((const uint8_t *) CV_V_MID);
        if ((high == 0) || (high == 1)) //indicates to ignore the value
            high = 255;
        
        if ((mid == 0) || (mid == 1)) //ignore it and recalculate;
            mid = (start + high) >> 1; //average of the two;
        
        //DCC_speed_table[0] = 0;
        DCC_speed_table[1] = start;
        uint32_t delta = mid - start;
        for (i = 2; i < 63; ++i)
        {
            //interpolate between vstart and vmid.
            //DCC_speed_table[i] = ((i - 1)*(delta)* 0.01613) + start; //1/(64-2)
            DCC_speed_table[i] = ((delta)* (i - 1) * 66) / 4096 + start;
        }
        DCC_speed_table[63] = mid;
        delta = high - mid;
        for (i = 64; i < 126; ++i)
        {
            //DCC_speed_table[i] = ((i - 63) * delta * 0.01587) + mid; //1/(127-64)
            DCC_speed_table[i] = (delta * (i - 63) * 65) / 4096 + mid;
        }
        DCC_speed_table[126] = high;
    }
    
    //finally, load up settings
    BEMF_cutoff = eeprom_read_byte((const uint8_t *) CV_EMF_FEEDBACK_CUTOUT);
    if (BEMF_cutoff) // no sense in loading the gains if not going to use!
    {
        BEMF_period = eeprom_read_byte((const uint8_t *) CV_BEMF_PERIOD);
        BEMF_Kp = eeprom_read_byte((const uint8_t *) CV_BEMF_PGAIN) + 1; //to make it a fraction out of 256
        BEMF_Ki = eeprom_read_byte((const uint8_t *) CV_BEMF_IGAIN) + 1;
        BEMF_Kf = eeprom_read_byte((const uint8_t *) CV_BEMF_FGAIN) + 1;
        BEMF_Kf_consist = eeprom_read_byte((const uint8_t *) CV_BEMF_FGAIN_CONSIST) + 1;
    }
    
}

void _load_FX(void)
{
    uint8_t i;
    for(i = 0; i < 2; ++i) //do this for each output
    {
        const uint8_t offset = 5*i;
        FX[i] = eeprom_read_byte((const uint8_t *) (CV_OUTPUT_1_DIRECTION_RULE17 + offset));
        FX_Brightness[i] = eeprom_read_byte((const uint8_t *) (CV_OUTPUT_1_BRIGHTNESS + offset));
        FX_Dim_Brightness[i] = eeprom_read_byte((const uint8_t *) (CV_OUTPUT_1_RULE_17_BRIGHTNESS + offset));
        FX_Animation[i] = eeprom_read_byte((const uint8_t *) (CV_OUTPUT_1_FX + offset));
        if( (FX_Animation[i] & ~FX_TRIGGERABLE_MASK) >= FX_NUM_ANIMATIONS)
            FX_Animation[i] = (FX_Animation[i] & FX_TRIGGERABLE_MASK) | (FX_NUM_ANIMATIONS -1);
#ifdef __AEGAEON_C
		//for some reason, this is royally f'd up when we use OCRB to drive PWM directly! This calculation restores the balance. UGLY! FIND HARDWARE FIX!
		uint16_t temp = eeprom_read_byte((const uint8_t*)(CV_OUTPUT_1_PERIOD + offset));
		temp = temp * 62 / 100;
		FX_Period[i] = (uint8_t)temp;
#else
		FX_Period[i] = eeprom_read_byte((const uint8_t*)(CV_OUTPUT_1_PERIOD + offset));
#endif
    }
}

void DCC_Config_Initialize(void)
//preps EEPROM, loads necessary persistent CV data, etc.
{
    //if this is the first time being called, we have some initialization to do!
    uint8_t i;
    
    switch (eeprom_read_byte((const uint8_t *) 0x00))
    {
        case 0xFF:
            for (i = CV_SPEED_TABLE_1; i <= CV_SPEED_TABLE_28; ++i)
            {
                eeprom_update_byte((uint8_t *) i, _defaults[i]); //eeprom_read_byte((const uint8_t*) (i + CV_DEFAULT_OFFSET)));
            }
            //fall through
        case 0xFE:
            _initialize_eeprom();
    }
    DCC_Config_Load_RAM_Cache();
}

void DCC_Config_Verify_CV(uint8_t CV, uint8_t value)
{
    if(CV > CV_USER_ID_2) return;
    uint8_t key = eeprom_read_byte((const uint8_t *) CV_DECODER_LOCK_KEY);
    uint8_t lock = eeprom_read_byte((const uint8_t *) CV_DECODER_LOCK_LOCK);
    if((key == lock) || !lock || !key ) //if decoder is locked, and key is not set to 0
    {
        if (eeprom_read_byte((const uint8_t *) CV) == value)
        {
            Motor_Jog();
        }
    }
}

void DCC_Config_Write_CV( uint8_t CV, uint8_t value )
{
    uint8_t lock = eeprom_read_byte((const uint8_t *) CV_DECODER_LOCK_LOCK);
    if( !lock || (lock == eeprom_read_byte((const uint8_t *) CV_DECODER_LOCK_KEY)) ||  (CV == CV_DECODER_LOCK_KEY)) //if decoder is locked, and key is not set to broadcast
    {
        uint16_t CV_temp = CV;
        uint8_t new_value = value;
        //        if (CV_temp > CV_USER_ID_2) //we don't implement any CV_temp beyond 106
        //            return 0;
        //Check against reserved ranges
        
        if ((CV_temp == 20) || (CV_temp == 26) || (CV_temp == 28) || (CV == 33) || ((CV_temp >= 96) && (CV_temp <= 104)) ||  (CV_temp == CV_MANUFACTURER_VERSION) ||  (CV_temp == CV_FIRMWARE_REVISION) ||  (CV_temp > CV_USER_ID_2) ) //reserved ranges
            return;
        
        //TODO check to see if this is a RO value, in which case, don't write!
        //        else if (CV_temp == CV_MANUFACTURER_VERSION)
        //            return 0; //TODO What else?
        
        //Is this a reset command?
        else if (CV_temp == CV_MANUFACTURER_ID)
        {
            if (value == 0x08)
            {
                eeprom_update_byte((uint8_t *) 0, 0xFF);
                DCC_Config_Initialize();
                Motor_Jog();
            }
            else if (value == 0x09)
            {
                eeprom_update_byte((uint8_t *) 0, 0xFE); //preserve speed tables
                DCC_Config_Initialize();
                Motor_Jog();
            }
            return;
        }
        
        else if (CV_temp == 29) //special rules prevent us from modifying unsupported bits
        {
            new_value |= new_value & CV29_MASK;
        }
        
        else if (CV_temp == CV_PRIMARY_ADDRESS)
        {
            if ((new_value > 127) || (new_value == 0)) //invalid!!
                return;
            //we must also set a bit in CV29, and clear CV19! RP9.2.3, line 141
            eeprom_update_byte((uint8_t *) 29, DCC_CV29 & ~0x20);
            eeprom_update_byte((uint8_t *) 19, 0x00);
        }
        
        else if (CV_temp == CV_EXTENDED_ADDRESS_LSB)
        {
            //pull CV_temp 17 from cache
            uint8_t MSB = eeprom_read_byte((const uint8_t *) CV_EXTENDED_ADDRESS_MSB_TEMP);
            if ((MSB == 0) && (new_value == 0)) //zero address, invalid! Ignore
                return;
            
            eeprom_update_byte((uint8_t *) CV_EXTENDED_ADDRESS_MSB, MSB);
            //then let the LSB be written naturally.
        }
        
        else if (CV_temp == CV_EXTENDED_ADDRESS_MSB) //cache this one, don't write it directly
        {
            CV_temp = CV_EXTENDED_ADDRESS_MSB_TEMP; //stash it somewhere else
        }
        
        //if we made it to here, just write the damned byte, then issue a response
        eeprom_update_byte((uint8_t *) CV_temp, new_value);
        if (eeprom_read_byte((const uint8_t *) CV_temp) != new_value) //make sure the write took
        {
            return; //write failed, bail
        }
        
        //Refresh the RAM cache
        DCC_Config_Load_RAM_Cache();
        
        //made it this far, let's give set up ack pulse if configured to do so
        Motor_Jog();
    }
}

void DCC_Config_Write_CV_Bit(uint8_t CV, uint8_t val, uint8_t pos)
{
    uint8_t cur_val = eeprom_read_byte((const uint8_t *) CV);
    if (val) //set bit
        cur_val |= (1 << pos);
    else
        cur_val &= ~(1 << pos);
    DCC_Config_Write_CV(CV, cur_val);
}

void DCC_Config_Verify_CV_Bit(uint8_t CV, uint8_t val, uint8_t pos)
{
    uint8_t cur_val = eeprom_read_byte((const uint8_t *) CV);
    if ((cur_val & (1 << pos)) == (val << pos))
        Motor_Jog();
}

void DCC_Config_Load_RAM_Cache(void)
{
    //here is where everything in EEPROM that we need frequent access to gets loaded
    
    DCC_CV29 = eeprom_read_byte((const uint8_t *) CV_CONFIGURATION_DATA);
    _load_address();
    
    _load_speed_table();
    
    //load FX
    _load_FX();
}
