#include "DCC_Packet.h"
#include "DCC_Config.h"
#include "DCC_Decoder.h"
#include "globals.h"
#include <stdint.h>


static const uint8_t twentyeight_step_lookup[] = {
    1, //stop
    1, //estop
    2, //step 1
    4,
    6,
    8,
    10,
    12,
    14,
    16,
    18,
    20,
    22,
    24,
    26,
    28,

    1, //stop
    1, //estop
    3, //step 2
    5,
    7,
    9,
    11,
    13,
    15,
    17,
    19,
    21,
    23,
    25,
    27,
    29
};

void DCC_Packet_Init(DCC_Packet_t *packet)
{
    packet->data[0] = 0xFF;
    packet->data[1] = 0x00;
    packet->data[2] = 0xFF;
    packet->data_start = 1;
    packet->size = 3;
    packet->kind = DCC_PACKET_IDLE_KIND;
}

void DCC_Packet_Zero(DCC_Packet_t *packet)
{
    packet->data[0] = 0x00;
    packet->data[1] = 0x00;
    packet->data[2] = 0x00;
    packet->data[3] = 0x00;
    packet->data[4] = 0x00;
    packet->data[5] = 0x00;
    packet->data_start = 0;
    packet->size = 0;
    packet->kind = DCC_PACKET_NONE_KIND;
}

void getCVMask(DCC_Packet_t *packet, uint8_t i)
{
  switch (packet->data[i] & 0x0C)
  {
  case 0x04: //verify byte
      packet->kind |= CV_VERIFY_KIND_MASK;
      break;
  case 0x0C:
      packet->kind |= CV_WRITE_KIND_MASK;
      break;
  case 0x08: //bit manipulation, a little trickier
      packet->kind |= CV_BIT_MANIPULATION_MASK;
      if(packet->data[i+2] & 0x10)
          packet->kind |= CV_WRITE_KIND_MASK;
      else
          packet->kind |= CV_VERIFY_KIND_MASK;
      break;
  }
}

//returns the address of a DCC operations mode packet
// returns 0 if it is for an accessory decoder, and should therefore be ignored
// returns 0xFF for broadcast packets of all kinds
//not valid on service mode packets; see service mode routines in DCC_Service.h

uint8_t DCC_Packet_Get_Address(DCC_Packet_t *packet, uint16_t *address)
{
     *address = 0x0000;
    //first, is this an accessory decoder packet? If so, ignore it
    //if ((packet->data[0] >> 6) == 0x02)
    //{
    //    return DCC_ACCESSORY_ADDRESS;
    //}
    //second, is this a broadcast packet?
    if ((packet->data[0] == 0xFF) || (packet->data[0] == 0x00)) //broadcast
    {
        if (DCC_consist_address & 0x7F) //if we are in a consist, use that address
            *address = DCC_consist_address;
        else
            *address = DCC_address; //force it to be us
        return DCC_address_kind;
    }
    //finally, is this a 2-digit or 4-digit address, or a broadcast packet?
    if ((packet->data[0] & 0xC0) == 0xC0) //bit mask to indicate extended address
    {
        if (packet->data[0] > 0xE7) //longest possible long address, any longer is in the reserverd range, or is an idle packet
        {
            return DCC_SHORT_ADDRESS; //used as an ignore indicator
        }
        *address = (uint16_t) (packet->data[0] & 0x3F) << 8;
        *address |= (uint16_t) (packet->data[1]);
        return DCC_LONG_ADDRESS;
    }
    else //short address
    {
        *address = packet->data[0];
        return DCC_SHORT_ADDRESS;
    }

    //should never get here
    return DCC_SHORT_ADDRESS;
}

//void DCC_Packet_Get_Data_Start(DCC_Packet_t *packet)
//{
//    packet->data_start = 1;
//    if (((packet->data[0] & 0xC0) == 0xC0) && (packet->data[0] <= 0xE7)) //long address, covers two bytes.
//        packet->data_start = 2;
//}

void DCC_Service_Mode_Packet_Get_Kind(DCC_Packet_t *packet)
{
    packet->kind = DCC_PACKET_NONE_KIND;

    //Direct:
    // 0 0111CCAA 0 AAAAAAAA 0 DDDDDDDD 0 EEEEEEEE 1
    //Page-Preset:
    // 0 01111101 0 00000001 0 01111100 1
    //Address-Only:
    // 0 0111C000 0 0DDDDDDD 0 EEEEEEEE 1
    //Physical Register:
    // 0 0111CRRR 0 DDDDDDDD 0 EEEEEEEE 1
    //Paged
    // 0 0111CRRR 0 DDDDDDDD 0 EEEEEEEE 1
    //notice that all begin with 0x70 in first byte
    if ((packet->data[0] & 0x70) == 0x70)
    {
        packet->kind = SERVICE_MODE_KIND_MASK; //just in case it is something we don't handle here
        if(packet->size == 4)
        {
          packet->kind |= SERVICE_MODE_DIRECT_KIND; //assume this for now
          getCVMask(packet, 0);
        }
        else if(packet->size == 3)
        {
            packet->kind |= SERVICE_MODE_PAGED_KIND;
            switch (packet->data[0] & 0x08)
            {
            case 0x00: //verify byte
                packet->kind |= CV_VERIFY_KIND_MASK;
                break;
            case 0x01:
                packet->kind |= CV_WRITE_KIND_MASK;
                break;
            }
        }
    }

    return;// packet->kind;
}

void DCC_Packet_Get_Kind( DCC_Packet_t *packet )
{
    packet->kind = DCC_PACKET_NONE_KIND;

    DCC_Service_Mode_Packet_Get_Kind(packet);
    if (DCC_SERVICE_MODE && packet->kind)
        return; // packet->kind;


//    DCC_Packet_Get_Data_Start(packet);
    packet->data_start = 1;
    if (((packet->data[0] & 0xC0) == 0xC0) && (packet->data[0] <= 0xE7)) //long address, covers two bytes.
    packet->data_start = 2;

    //Baseline packets from S-9.2
    // broadcast reset packet
    // 00000000 00000000 00000000
    /*** handled below
     if ((packet->size == 3) && (packet->data[0] == 0x00)
     && (packet->data[1] == 0x00) && (packet->data[2] == 0x00))
     {
     packet->kind = DCC_PACKET_RESET_KIND;
     return packet->kind;
     }
     ***/

    // broadcast idle packet
    // 11111111 0000000 11111111
    if (packet->data[0] == 0xFF)
    {
        packet->kind = DCC_PACKET_NONE_KIND; //DCC_PACKET_IDLE_KIND;
        return; // packet->kind;
    }

    //if ((packet->size == 3) && (packet->data[0] == 0xFF)
    //        && (packet->data[1] == 0x00))
    //{
    //    packet->kind = DCC_PACKET_IDLE_KIND;
    //}
    // broadcast powered-stop packet
        // 00000000 01DC0000 EEEEEEEE
        //          11001111 <- mask
//this section is redundant
//    else if ((packet->size == 3) && (packet->data[0] == 0x00)
//            && ((packet->data[1] & 0xCF) == 0x41))
//    {
//        packet->kind = DCC_PACKET_E_STOP_KIND;
//    }
    //else //advanced packet types from RP 9.2.1
    //{
        //check for advanced instructions
        uint8_t header = packet->data[packet->data_start];
        switch ((header & 0xE0) >> 5)
        {
            // CCC = 000, Decoder and Consist Control Instruction
        case 0x00:
            switch (header & 0x10)
            {
            case 0x00: //decoder control
                switch ((header >> 1) & 0x07)
                {
                case 0x00: //reset kinds
                    if (header & 0x01)
                        packet->kind = DCC_PACKET_HARD_RESET_KIND;
                    else
                        packet->kind = DCC_PACKET_SOFT_RESET_KIND;
                    break;
//                case 0x05: //set advanced addressing
//                    packet->kind = DCC_PACKET_SET_ADVANCED_ADDRESSING_KIND;
//                    break;
//                case 0x07: //decoder ack request
//                    packet->kind = DCC_PACKET_ACK_REQUEST_KIND;
//                    break;
                }
                break;
            case 0x10: //Consist control
                if (packet->size >= 4)
                {
                    switch (header & 0x0F)
                    {
                    case 0x02:
                        packet->kind = DCC_PACKET_CREATE_CONSIST_FACE_FORWARD_KIND;
                        break;
                    case 0x03:
                        packet->kind = DCC_PACKET_CREATE_CONSIST_FACE_REVERSE_KIND;
                        break;
                    }
                    break;
                }
            }
            break;
            // CCC = 001, Advanced Operation Instruction
        case 0x01:
            switch (header & 0x1F)
            {
            case 0x1F: //128 speed step control
                if ((packet->data[packet->data_start + 1] & 0x7F) == 0x01)
                {
                    //really an e-stop
                    packet->kind = DCC_PACKET_E_STOP_KIND;
                }
                else
                {
                    packet->kind = DCC_PACKET_ADVANCED_SPEED_KIND;
                }
                break;
//            case 0x1E: //Restricted Speed Step Instruction
//                packet->kind = DCC_PACKET_RESTRICTED_SPEED_KIND;
//                Motor_Set_Restriction(packet->data[packet->data_start + 1] & 0x1F);
//                break;
            }
            break;
            // CCC = 010, Speed and Direction Instruction for reverse operation
            // CCC = 011, Speed and Direction Instruction for forward operation
        case 0x02:
        case 0x03:
            if ((packet->data[packet->data_start] & 0x0F) == 0x01) //estop!
                packet->kind = DCC_PACKET_E_STOP_KIND;
            else
                packet->kind = DCC_PACKET_SPEED_KIND;
            break;
            // CCC = 100, Function Group One Instruction
        case 0x04:
            packet->kind = DCC_PACKET_FUNC_FL_F4_KIND;
            break;
            // CCC = 101, Function Group Two Instruction
        case 0x05:
            if (header & 0x10)
                packet->kind = DCC_PACKET_FUNC_F5_F8_KIND;
            else
                packet->kind = DCC_PACKET_FUNC_F9_F12_KIND;
            break;
            // CCC = 110, Future Expansion
//        case 0x06:
            //more processing to do to determine sub-type
//            switch (packet->data[packet->data_start] & 0x1F)
                //check last 5 bits
//            {
//            case 0x00: //Binary State Control Instruction Long Form
//                packet->kind = DCC_PACKET_BINARY_STATE_LONG_KIND;
//                break;
//            case 0x1D: //Binary State Control Instruction Short Form
//                packet->kind = DCC_PACKET_BINARY_STATE_SHORT_KIND;
//                break;
//            case 0x1E: //F13-F20 Function Control
//                packet->kind = DCC_PACKET_FUNC_F13_F20_KIND;
//                break;
//            case 0x1F: //F21-F28 Function Control
//                packet->kind = DCC_PACKET_FUNC_F21_F28_KIND;
//                break;
//            }
//            break;
            // CCC = 111, Configuration Variable Access Instruction
        case 0x07:
//            if (header & 0x10)
//                packet->kind = DCC_PACKET_OPS_MODE_SHORT_CONFIG_KIND;
//            else
              if(0 == (header & 0x10))
              {
                packet->kind = DCC_PACKET_OPS_MODE_LONG_CONFIG_KIND;
                getCVMask(packet, 1);
              }
              break;
        }
    //}

    return;// packet->kind;
}

uint8_t DCC_Packet_Check_CRC(DCC_Packet_t *packet)
{
    uint8_t computed_XOR = 0;
    uint8_t i;
    if (packet->size < 3) //all packets are at least 3 bytes: address/data/CRC
        return 0;
    for (i = 0; i < packet->size - 1; ++i)
    {
        computed_XOR ^= packet->data[i];
    }
    return (computed_XOR == packet->data[packet->size - 1]);
}

int8_t DCC_Packet_Get_Speed(DCC_Packet_t *packet)
{
    //TODO two kinds of packets.
    int8_t retval = 0x00;
    if (DCC_PACKET_SPEED_KIND == packet->kind)
    {
        //TODO currently check CV29 in motor; really, should be done here!
        if (0x00 == (packet->data[packet->data_start] & 0x1F)) //regular stop
            retval = 1;
        retval = twentyeight_step_lookup[packet->data[packet->data_start] & 0x1F];
        if (~packet->data[packet->data_start] & 0x20) //check for direction bit; 0 = reverse
            retval *= -1;
    }
    else if (DCC_PACKET_ADVANCED_SPEED_KIND == packet->kind)
    {
        if (packet->size <= 3)
            return 0; //ERROR!

        else if (0x00 == (packet->data[packet->data_start + 1] & 0x7F)) //regular stop
            retval = 1;
        else
			retval = (packet->data[packet->data_start + 1] & 0x7F); //Yields a speed in range 2--127, which is what we want, since we use 1 == stop

        if (~packet->data[packet->data_start + 1] & 0x80) //reverse
            retval *= -1;
        //TODO Check CV29 here. Really. Instead of in motor.
    }
    else if (DCC_PACKET_E_STOP_KIND == packet->kind)
    {
        retval = 1;
        //now, find sign
        if (3 == packet->size) //basic
        {
            //case 3:
            if (~packet->data[packet->data_start] & 0x20) //check for direction bit; 0 = reverse
                retval *= -1;
                //break;

        }
        else if (4 == packet->size) //advanced
        {
            //case 4:
            if (~packet->data[packet->data_start + 1] & 0x80) //reverse
                retval *= -1;
        }
    }

    if (CV29_REVERSE) //if dir is reversed by CV29
        retval *= -1;
    if ((DCC_consist_address & 0x7F) && (DCC_consist_address & 0x80)) //if dir is reversed by CV19. Notice that this may cancel out CV29 setting, and that is correct behavior
        retval *= -1;
    //if ((DCC_consist_address & 0x7F) && ((DCC_consist_address & 0x80) == 0)) //if in a consist, and reverse facing bit is set, reverse speed
    //    retval *= -1;
    return retval;
}

/*
void DCC_Packet_Get_Function(DCC_Packet_t *packet, uint32_t* func)
{
    switch (packet->kind)
    {
    case DCC_PACKET_FUNC_FL_F4_KIND:
        *func &= ~DCC_PACKET_FUNC_F0_F4_MASK;
        *func |= packet->data[packet->data_start] & 0x1F;
        break;
    case DCC_PACKET_FUNC_F5_F8_KIND:
        *func &= ~DCC_PACKET_FUNC_F5_F8_MASK;
        *func |= (packet->data[packet->data_start] & 0xF) << 5;
        break;
    case DCC_PACKET_FUNC_F9_F12_KIND:
        *func &= ~DCC_PACKET_FUNC_F9_F12_MASK;
        *func |= (packet->data[packet->data_start] & 0xF) << 9;
        break;
    case DCC_PACKET_FUNC_F13_F20_KIND:
        if (packet->size > 3)
        {
            *func &= ~DCC_PACKET_FUNC_F13_F20_MASK;
            *func |= (packet->data[packet->data_start + 1]) << 13;
        }
        break;
    case DCC_PACKET_FUNC_F21_F28_KIND:
        if (packet->size > 3)
        {
            *func &= ~DCC_PACKET_FUNC_F21_F28_MASK;
            *func |= (packet->data[packet->data_start + 1]) << 21;
        }
        break;
    }
}
*/

/*uint8_t DCC_Packet_Equal(DCC_Packet_t *a, DCC_Packet_t *b)
{
    if ((a->kind != b->kind) || (a->size != b->size))
    {
        return 0;
    }

    uint8_t i;
    for (i = 0; i < a->size; ++i)
    {
        if (a->data[i] != b->data[i])
            return 0;
    }
    return 1;
}

void DCC_Packet_Copy(DCC_Packet_t *dest, DCC_Packet_t *src)
{
    dest->size = src->size;
    dest->kind = src->kind;
    dest->data_start = src->data_start;
    uint8_t i;
    for(i=0; i < 6; ++i)
        dest->data[i] = src->data[i];
}*/

//uint16_t DCC_Packet_Service_Get_CV(DCC_Packet_t *packet)
//{
//
//}
//
//uint8_t DCC_Packet_Service_Get_Value(DCC_Packet_t *packet)
//{
//
//}
