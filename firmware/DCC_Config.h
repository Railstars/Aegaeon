
#include <stdint.h>

#ifndef __DCC_CONFIG_H__
#define __DCC_CONDIG_H__

//Pick the decoder you want here, or do it with a -D at compile time
#define __AEGAEON_L2
//#define __AEGAEON_M
//#define __AEGAEON_C


#ifdef __AEGAEON_L2
#define DECODER_VERSION	0x05 //Aegaeon:L2
#else
#ifdef __AEGAEON_M
#define DECODER_VERSION 0x01 //Aegaeon:M
#else
#ifdef __AEGAEON_C
#define DECODER_VERSION 0x04 //Aegaeon:C
#else
#error No decoder defined!
#endif
#endif
#endif

#define MANUFACTURER_ID	0x5B //Railstars Limited
#define FIRMWARE_REVISION 0x01


//CV 29 helpers
#define CV29_REVERSE			(DCC_CV29 & 0x01)
#define CV29_28_SPEEDSTEPS		(DCC_CV29 & 0x02)
#define CV29_POWER_CONVERSION   (DCC_CV29 & 0x04)
#define CV29_RAILCOM			(DCC_CV29 & 0x08)
#define CV29_FANCY_SPEED_CURVE  (DCC_CV29 & 0x10)
#define CV29_LONG_ADDRESSING    (DCC_CV29 & 0x20)
#define CV29_RESERVED1			(DCC_CV29 & 0x40)
#define CV29_RESERVED2			(DCC_CV29 & 0x80)

#ifdef RAILCOM
#define CV29_MASK               (CV29_REVERSE | CV29_POWER_CONVERSION | CV29_FANCY_SPEED_CURVE | CV29_LONG_ADDRESSING)
#else
#define CV29_MASK               0x35
#endif

//CV definitions
#define CV_PRIMARY_ADDRESS		1	//mandatory,	3,	RW,	Uniform
#define CV_V_START			2	//recommended,	x,	RW
#define CV_ACCELERATION_RATE		3	//recommended,	x,	RW
#define CV_DECELERATION_RATE		4	//recommended,	x,	RW
#define CV_V_HIGH			5	//optional,		x,	RW
#define CV_V_MID			6	//optional,		x,	RW
#define CV_MANUFACTURER_VERSION         7	//mandatory,	x,	RO
#define CV_MANUFACTURER_ID		8	//mandatory,	x,	RO, Uniform
//#define CV_TOTAL_PWM_PERIOD		9	//optional,		x,	RW
#define CV_EMF_FEEDBACK_CUTOUT		10	//required,		x,	RW

#define CV_PACKET_TIME_OUT_VALUE	11	//optional,		x,	RW
//#define CV_POWER_SOURCE_CONVERSION	12	//optional,		x,	RW, Uniform
#define CV_ALTERNATE_MODE_FUNCTION_STATUS_F1_F8		13	//optional,		x,	RW, Uniform
#define CV_ALTERNATE_MODE_FUNCTION_STATUS_FL_F9_F12	14	//optional,		x,	RW, Uniform
#define CV_DECODER_LOCK_KEY		15	//optional,		x,	RW, Uniform
#define CV_DECODER_LOCK_LOCK		16	//optional,		x,	RW, Uniform
#define CV_EXTENDED_ADDRESS_MSB		17	//optional,		x,	RW, Uniform
#define CV_EXTENDED_ADDRESS_LSB		18	//optional,		x,	RW, Uniform
#define CV_CONSIST_ADDRESS			19	//optional,		x,	RW, Uniform
#define CV_RESERVED_20				20	//RESERVED

#define CV_CONSIST_ADDRESS_ACTIVE_FOR_F1_F8     21	//optional,		x,	RW, Uniform
#define CV_CONSIST_ADDRESS_ACTIVE_FOR_FL_F9_F12	22	//optional,		x,	RW, Uniform
//#define CV_ACCELERATION_ADJUSTMENT	23	//optional,		x,	RW, Uniform
//#define CV_DECELERATION_ADJUSTMENT	24	//optional,		x,	RW, Uniform
//#define CV_SPEED_TABLE				25	//optional,		x,	RW, Uniform
//#define CV_MID_RANGE_SPEED_STEP		25	//optional,		x,	RW, Uniform //same as the one above, but has two purposes
#define CV_RESERVED_26				26	//RESERVED
//#define CV_DECODER_AUTOMATIC_STOPPING_CONFIG		27	//optional,		x,	RW, Uniform
//#define CV_BI_DIRECTIONAL_COMMUNICIMATION_CONFIG		28	//optional,		x,	RW, Uniform
#define CV_CONFIGURATION_DATA		29	//mandatory*,	x,	RW, Uniform
//#define CV_ERROR_INFORMATION		30	//optional,		x,	RW, Uniform

//#define CV_INDEX_HIGH_BYTE			31	//optional,		x,	RW, Uniform
//#define CV_INDEX_LOW_BYTE			32	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FLF		33	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FLR		34	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL1		35	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL2		36	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL3		37	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL4		38	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL5		39	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL6		40	//optional,		x,	RW, Uniform

#define CV_OUTPUT_LOCATION_FL7		41	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL8		42	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL9		43	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL10		44	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL11		45	//optional,		x,	RW, Uniform
#define CV_OUTPUT_LOCATION_FL12		46	//optional,		x,	RW, Uniform

#define CV_OPS_MODE_BASIC_ACK           47

#define CV_OUTPUT_1_DIRECTION_RULE17    48
#define CV_OUTPUT_1_BRIGHTNESS  	    49	//optional,		x,	RW
#define CV_OUTPUT_1_RULE_17_BRIGHTNESS  50	//optional,		x,	RW
#define CV_OUTPUT_1_FX                  51	//optional,		x,	RW
#define CV_OUTPUT_1_PERIOD              52

#define CV_OUTPUT_2_DIRECTION_RULE17    53
#define CV_OUTPUT_2_BRIGHTNESS          54
#define CV_OUTPUT_2_RULE_17_BRIGHTNESS  55
#define CV_OUTPUT_2_FX                  56	//optional,		x,	RW
#define CV_OUTPUT_2_PERIOD              57

#define CV_FIRMWARE_REVISION		58
#define CV_MOTOR_FREQUENCY      		59

#define CV_BEMF_PERIOD	60	//optional,		x,	RW
#define CV_BEMF_PGAIN	61	//optional,		x,	RW
#define CV_BEMF_IGAIN	62	//optional,		x,	RW
#define CV_BEMF_FGAIN	63	//optional,		x,	RW
#define CV_BEMF_FGAIN_CONSIST	64	//optional,		x,	RW

#define CV_KICK_START			65	//optional,		x,	RW
#define CV_FORWARD_TRIM			66	//optional,		x,	RW
#define CV_SPEED_TABLE_1		67	//optional,		x,	RW
#define CV_SPEED_TABLE_2		68	//optional,		x,	RW
#define CV_SPEED_TABLE_3		69	//optional,		x,	RW
#define CV_SPEED_TABLE_4		70	//optional,		x,	RW

#define CV_SPEED_TABLE_5		71	//optional,		x,	RW
#define CV_SPEED_TABLE_6		72	//optional,		x,	RW
#define CV_SPEED_TABLE_7		73	//optional,		x,	RW
#define CV_SPEED_TABLE_8		74	//optional,		x,	RW
#define CV_SPEED_TABLE_9		75	//optional,		x,	RW
#define CV_SPEED_TABLE_10		76	//optional,		x,	RW
#define CV_SPEED_TABLE_11		77	//optional,		x,	RW
#define CV_SPEED_TABLE_12		78	//optional,		x,	RW
#define CV_SPEED_TABLE_13		79	//optional,		x,	RW
#define CV_SPEED_TABLE_14		80	//optional,		x,	RW

#define CV_SPEED_TABLE_15		81	//optional,		x,	RW
#define CV_SPEED_TABLE_16		82	//optional,		x,	RW
#define CV_SPEED_TABLE_17		83	//optional,		x,	RW
#define CV_SPEED_TABLE_18		84	//optional,		x,	RW
#define CV_SPEED_TABLE_19		85	//optional,		x,	RW
#define CV_SPEED_TABLE_20		86	//optional,		x,	RW
#define CV_SPEED_TABLE_21		87	//optional,		x,	RW
#define CV_SPEED_TABLE_22		88	//optional,		x,	RW
#define CV_SPEED_TABLE_23		89	//optional,		x,	RW
#define CV_SPEED_TABLE_24		90	//optional,		x,	RW

#define CV_SPEED_TABLE_25		91	//optional,		x,	RW
#define CV_SPEED_TABLE_26		92	//optional,		x,	RW
#define CV_SPEED_TABLE_27		93	//optional,		x,	RW
#define CV_SPEED_TABLE_28		94	//optional,		x,	RW
#define CV_REVERSE_TRIM			95	//optional,		x,	RW
//#define CV_RESERVED			96	//RESERVED
//#define CV_RESERVED			97	//RESERVED
//#define CV_RESERVED			98	//RESERVED
//#define CV_RESERVED			99	//RESERVED
//#define CV_RESERVED			100	//RESERVED

//#define CV_RESERVED			101	//RESERVED
//#define CV_RESERVED			102	//RESERVED
//#define CV_RESERVED			103	//RESERVED
//#define CV_RESERVED			104	//RESERVED
#define CV_USER_ID_1			105	//optional,		x,	RW
#define CV_USER_ID_2			106	//optional,		x,	RW
//#define CV_RESERVED			107	//RESERVED
//#define CV_RESERVED			108	//RESERVED
//#define CV_RESERVED			109	//RESERVED
//#define CV_RESERVED			110	//RESERVED

//#define CV_RESERVED			111	//RESERVED
//Manufacturer unique 112-256
//indexed area 257-512
//reserved CVs 513-879
//reserved CVs 880-891
//dynamic CVs 892, 893, 894, 895
//SUSI sound and function modules 896-1024 TN-9.2.3

#define CV_PAGE 108
#define CV_EXTENDED_ADDRESS_MSB_TEMP 107
#define DCC_SHORT_ADDRESS 0
#define DCC_LONG_ADDRESS 1
#define DCC_ACCESSORY_ADDRESS 2
#define DCC_ERROR_ADDRESS 3

void DCC_Config_Initialize(void); //preps EEPROM, loads necessary persistent CV data, etc.
//uint8_t DCC_Config_Read_CV(uint8_t CV);
void DCC_Config_Write_CV(uint8_t CV, uint8_t value);
void DCC_Config_Verify_CV(uint8_t CV, uint8_t value);
void DCC_Config_Write_CV_Bit(uint8_t CV, uint8_t val, uint8_t pos);
void DCC_Config_Verify_CV_Bit(uint8_t CV, uint8_t val, uint8_t pos);
void DCC_Config_Load_RAM_Cache(void);
void DCC_Config_Hard_Reset(void);

#endif //__DCC_CONFIG_H__
