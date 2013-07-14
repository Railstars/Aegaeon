#ifndef __DCC_PACKET_H__
#define __DCC_PACKET_H__

#include <stdint.h>

#define SERVICE_MODE_KIND_MASK                                  0x80
#define CV_WRITE_KIND_MASK                                      0x40
#define CV_VERIFY_KIND_MASK                                     0x20
#define CV_BIT_MANIPULATION_MASK                                0x01
#define DCC_PACKET_SOFT_RESET_KIND                              0x9E //both service mode and ops mode
#define DCC_PACKET_HARD_RESET_KIND				0x9F //both service mode and ops mode
#define DCC_PACKET_OPS_SOFT_RESET_KIND                          0x1E //both service mode and ops mode
#define DCC_PACKET_OPS_HARD_RESET_KIND				0x1F //both service mode and ops mode



#define DCC_PACKET_NONE_KIND                                    0x00
#define DCC_PACKET_IDLE_KIND					                0x00
#define DCC_PACKET_E_STOP_KIND					                0x01
#define DCC_PACKET_SPEED_KIND					                0x02
#define DCC_PACKET_ADVANCED_SPEED_KIND                  	    0x03
#define DCC_PACKET_RESTRICTED_SPEED_KIND                	    0x04
#define DCC_PACKET_FUNC_FL_F4_KIND				                0x05
#define DCC_PACKET_FUNC_F5_F8_KIND				                0x06
#define DCC_PACKET_FUNC_F9_F12_KIND				                0x07
#define DCC_PACKET_FUNC_F13_F20_KIND                    	    0x08
#define DCC_PACKET_FUNC_F21_F28_KIND                    	    0x09
#define DCC_PACKET_BINARY_STATE_LONG_KIND       		        0x0A
#define DCC_PACKET_BINARY_STATE_SHORT_KIND      		        0x0B
#define DCC_PACKET_SET_ADVANCED_ADDRESSING_KIND                 0x0C
#define DCC_PACKET_ACK_REQUEST_KIND                             0x0D
#define DCC_PACKET_CREATE_CONSIST_FACE_FORWARD_KIND             0x0E
#define DCC_PACKET_CREATE_CONSIST_FACE_REVERSE_KIND             0x0F

#define DCC_PACKET_OPS_MODE_LONG_CONFIG_KIND                    0x10
#define DCC_PACKET_OPS_MODE_LONG_CONFIG_BIT_MANIPULATION_KIND   0x11
#define DCC_PACKET_OTHER_KIND					                0x00 //treat as none kind

//service mode packet kinds

#define SERVICE_MODE_DIRECT_KIND                                0x02
#define SERVICE_MODE_DIRECT_BIT_MANIPULATION_KIND               0x03
#define SERVICE_MODE_PAGED_KIND                                 0x04

#define DCC_PACKET_SERVICE_DIRECT_WRITE_KIND                    0x92
#define DCC_PACKET_SERVICE_DIRECT_VERIFY_KIND                   0xA2

#define DCC_PACKET_SERVICE_DIRECT_BIT_MANIPULATION_WRITE_KIND   0x93
#define DCC_PACKET_SERVICE_DIRECT_BIT_MANIPULATION_VERIFY_KIND  0xA3

#define DCC_PACKET_SERVICE_PAGED_WRITE_KIND                     0x94
#define DCC_PACKET_SERVICE_PAGED_VERIFY_KIND                    0xA4

#define DCC_PACKET_SPEED_ERROR					0xFFFF
#define DCC_PACKET_SPEED_ESTOP					0xFFFE

#define DCC_FUNCTION_F0							(1<<0)
#define DCC_FUNCTION_F1							(1<<1)
#define DCC_FUNCTION_F2							(1<<2)
#define DCC_FUNCTION_F3							(1<<3)
#define DCC_FUNCTION_F4							(1<<4)

#define DCC_FUNCTION_F5							(1<<5)
#define DCC_FUNCTION_F6							(1<<6)
#define DCC_FUNCTION_F7							(1<<7)
#define DCC_FUNCTION_F8							(1<<8)

#define DCC_FUNCTION_F9							(1<<9)
#define DCC_FUNCTION_F10						(1<<10)
#define DCC_FUNCTION_F11						(1<<11)
#define DCC_FUNCTION_F12						(1<<12)

#define DCC_FUNCTION_F13						(1<<13)
#define DCC_FUNCTION_F14						(1<<14)
#define DCC_FUNCTION_F15						(1<<15)
#define DCC_FUNCTION_F16						(1<<16)
#define DCC_FUNCTION_F17						(1<<17)
#define DCC_FUNCTION_F18						(1<<18)
#define DCC_FUNCTION_F19						(1<<19)
#define DCC_FUNCTION_F20						(1<<20)

#define DCC_FUNCTION_F21						(1<<21)
#define DCC_FUNCTION_F22						(1<<22)
#define DCC_FUNCTION_F23						(1<<23)
#define DCC_FUNCTION_F24						(1<<24)
#define DCC_FUNCTION_F25						(1<<25)
#define DCC_FUNCTION_F26						(1<<26)
#define DCC_FUNCTION_F27						(1<<27)
#define DCC_FUNCTION_F28						(1<<28)

#define DCC_PACKET_FUNC_F0_F4_MASK                                         0x0000001F
#define DCC_PACKET_FUNC_F5_F8_MASK                                         0x000001E0
#define DCC_PACKET_FUNC_F9_F12_MASK                                        0x00001E00
#define DCC_PACKET_FUNC_F13_F20_MASK                                        0x001FE000
#define DCC_PACKET_FUNC_F21_F28_MASK                                       0x1FE00000

typedef struct
{
    uint8_t size;
    uint8_t data_start;
    uint8_t data[6];
    uint8_t kind;
} DCC_Packet_t;


void DCC_Packet_Init(DCC_Packet_t *packet);
void DCC_Packet_Get_Kind(DCC_Packet_t *packet);
uint8_t DCC_Packet_Get_Address(DCC_Packet_t *packet, uint16_t *address);
uint8_t DCC_Packet_Check_CRC(DCC_Packet_t *packet);
int8_t DCC_Packet_Get_Speed(DCC_Packet_t *packet);
void DCC_Packet_Zero(DCC_Packet_t *packet);
//void DCC_Packet_Get_Function(DCC_Packet_t *packet, uint32_t* func);
//uint8_t DCC_Packet_Equal(DCC_Packet_t *a, DCC_Packet_t *b);
//void DCC_Packet_Copy(DCC_Packet_t *dest, DCC_Packet_t *src);
//uint16_t DCC_Packet_Service_Get_CV(DCC_Packet_t *packet);
//uint8_t DCC_Packet_Service_Get_Value(DCC_Packet_t *packet);

#endif //__DCC_PACKET_H__
