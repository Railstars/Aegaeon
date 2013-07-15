#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include "reset.h"
#include "DCC_Decoder.h"
#include "DCC_Packet.h"
#include "DCC_Config.h"
#include "Motor.h"
#include "FX.h"
#include "globals.h"

#define CONSIST_MATCH 0x01
#define ADDRESS_MATCH 0x02

//TODO read time-out value from CV 11

uint8_t prev_packet_kind; //for keeping track of service mode packets
volatile uint8_t preamble_bits, input_state, byte_counter, bit_counter;
volatile uint32_t _time_of_last_packet;

#define DCC_PREAMBLE 0
#define DCC_DATA 1

//void DCC_Decoder_Initialize(void)
//{
    //	preamble_bits = 0;
    //	input_state = DCC_PREAMBLE;
    //	byte_counter = 0;
    //	bit_counter = 0;
    //	DCC_Packet_Zero(&(DCC_rx_buffer[0]));
    //	DCC_Packet_Zero(&(DCC_rx_buffer[1]));
    //	buf_sel = 0;
    //	ready_buf = 0;
    //	DCC_flags = 0;
    //	_time_of_last_packet = 0;
//}

#ifdef TESTING
extern uint8_t value;
#endif

void DCC_Decoder_Process_Bit(uint8_t bit_val)
{
    #ifdef TESTING
    value = bit_val;
    #endif
    //let's time this on the o-scope.
    if (input_state == DCC_PREAMBLE)
    {
        //printf("DCC_PREAMBLE\n");
        if (!bit_val)
        {
            if (preamble_bits >= 12)
            {
                //printf("preamble sync\n");
                //start processing
                byte_counter = 0;
                bit_counter = 0;
                input_state = DCC_DATA;
                preamble_bits = 0;
                return;
            }
            //else
            //printf("reset\n");
            preamble_bits = 0;
            return;
        }
        //else
        ++preamble_bits;
        return;
    }
    //else, state is DCC_DATA
    //printf("DCC_DATA\n");
    if (5 == byte_counter)
    {
        //uint8_t i;
        //current packet is full up, with more to come. error. reset
        DCC_Packet_Zero(&(DCC_rx_buffer[buf_sel]));
        input_state = DCC_PREAMBLE;
        return;
    }
    if (bit_counter >= 8)
    {
        //current byte is full, roll it over to the next one.
        ++byte_counter;
        if (1 == bit_val) //done with packet
        {
            if (byte_counter >= 3) //only inform decoder if we received a complete packet. Otherwise just reset.
            {
                preamble_bits = 1; //get a head start on that preamble ;)
                uint8_t i;
                //printf("packet complete\n");
                DCC_rx_buffer[buf_sel].size = byte_counter;
                //move down circular buffer and flag that the previous one is ready
                ready_buf = buf_sel;
                DCC_flags |= DCC_FLAGS_READY;
                buf_sel = buf_sel ? 0 : 1;
                //buf_sel = (buf_sel+1)%2;
                DCC_rx_buffer[buf_sel].size = 0; //TODO can shorten!
                for (i = 0; i < 6; ++i)
                DCC_rx_buffer[buf_sel].data[i] = 0;
            }
            else
            {
                DCC_Packet_Zero(&(DCC_rx_buffer[buf_sel]));
                ++preamble_bits;
            }
            //reset to watch for preamble

            input_state = DCC_PREAMBLE;
            return;
        }
        //else bit is a zero
        preamble_bits = 0;
        //printf("new byte\n");
        //else, not done with packet, only byte
        bit_counter = 0;
        return;
    }

    //else, middle of reading a bit
    ++bit_counter;
    if (1 == bit_val) //we have a 1
    {
        ++preamble_bits;
        DCC_rx_buffer[buf_sel].data[byte_counter] |= (bit_val << (8 - bit_counter));
    }
    else
    preamble_bits = 0;
    return;
}

void handleDirectModePacket(DCC_Packet_t *packet, uint8_t i)
{
    uint8_t CV;
    uint8_t val;

    if (packet->data[i] & 0x03) //CV address beyond 255 not supported
    return;

    CV = packet->data[i+1] + 1;

    if(packet->kind & CV_BIT_MANIPULATION_MASK)
    {
        uint8_t pos;
        val = (packet->data[i+2] & 0x08) >> 3;
        pos = packet->data[i+2] & 0x07;
        if ((packet->kind & CV_WRITE_KIND_MASK) == CV_WRITE_KIND_MASK)
        {
            DCC_Config_Write_CV_Bit(CV, val, pos);
        }
        else if ((packet->kind & CV_VERIFY_KIND_MASK) == CV_VERIFY_KIND_MASK)
        {
            DCC_Config_Verify_CV_Bit(CV, val, pos);
        }
    }

    else //regular write or verify kind
    {
        val = packet->data[i+2];
        if ((packet->kind & CV_WRITE_KIND_MASK) == CV_WRITE_KIND_MASK)
        {
            DCC_Config_Write_CV(CV, val);
        }
        else if ((packet->kind & CV_VERIFY_KIND_MASK) == CV_VERIFY_KIND_MASK)
        {
            DCC_Config_Verify_CV(CV, val);
        }
    }
}

void Ops_Mode_Process_Packet(DCC_Packet_t *packet)
{
    uint8_t tempa;//, tempb; //needed for function messages
    uint16_t add;
    uint8_t kind = DCC_Packet_Get_Address(packet, &add);
    uint8_t packet_match = 0;
    if ((DCC_consist_address & 0x7F) && (add == (DCC_consist_address & 0x7F)) && (kind == DCC_SHORT_ADDRESS)) //message matches our consist address, which is non-zero
    {
        packet_match = CONSIST_MATCH;
    }
    else if ((add == DCC_address) && (kind == DCC_address_kind))
    {
        packet_match = ADDRESS_MATCH;
    }
    else
    {
        return; //not addressed to us, ignore it. Notice that GetAddress reformats all broadcast packets so that they appear to be addressed to us.
    }

    prev_packet_kind = packet->kind;
    switch (packet->kind & 0x1F)
    {
        /**** Speed Commands ****/
        case DCC_PACKET_E_STOP_KIND:
        //check length
            Motor_EStop(DCC_Packet_Get_Speed(packet));
            break;
        case DCC_PACKET_ADVANCED_SPEED_KIND:
        //if consist_address, only respond if packet_match & CONSIST_MATCH
            if ((((DCC_consist_address & 0x7F) && (packet_match & CONSIST_MATCH)))
                || (!(DCC_consist_address & 0x7F))) //ditch it
                Motor_Set_Speed_By_DCC_Speed_Step_128(DCC_Packet_Get_Speed(packet));
            break;
        case DCC_PACKET_SPEED_KIND:
            if ((((DCC_consist_address & 0x7F) && (packet_match & CONSIST_MATCH)))
                || (!(DCC_consist_address & 0x7F))) //ditch it
                Motor_Set_Speed_By_DCC_Speed_Step_28(DCC_Packet_Get_Speed(packet));
            break;
        //   		case DCC_PACKET_RESTRICTED_SPEED_KIND:
        //    		if (packet->data[packet->data_start + 1] & 0x80) //disable restricted speed
        //        		Motor_Remove_Restriction();
        //    		else
        //	        	Motor_Set_Restriction(packet->data[packet->data_start + 1] & 0x1F);
        //   		break;


        /**** Function Commands ****/
        case DCC_PACKET_FUNC_FL_F4_KIND:
            //gotta rearrange a bit
            tempa = (packet->data[packet->data_start] & 0x0F) << 2;
            if (packet->data[packet->data_start] & 0x10) //if FL bit is set
            {
                tempa |= 0x03; //send both, ignore direction
            }
            FX_SetFunction(tempa, FX_GROUP_1, packet_match == CONSIST_MATCH);
            break;

        case DCC_PACKET_FUNC_F5_F8_KIND:
            tempa = packet->data[packet->data_start] & 0x0F;
            FX_SetFunction(tempa, FX_GROUP_2, packet_match == CONSIST_MATCH);
            break;
        //case  DCC_PACKET_FUNC_F9_F12_KIND:
        //    tempa = packet->data[packet->data_start] & 0x0F;
        //    FX_SetFunction(tempa, FX_GROUP_3);
        //    break;

        /**** Service Mode-Related Commands ****/
        case DCC_PACKET_OPS_HARD_RESET_KIND:
            DCC_Config_Hard_Reset();
            //fall through;
        case DCC_PACKET_OPS_SOFT_RESET_KIND:
            soft_reset();
            break;

        /**** Ops Mode Programming ****/
        //		case DCC_PACKET_OPS_MODE_SHORT_CONFIG_KIND:
        //    		if ((packet->data[packet->data_start] & 0x0F) == 0x02)
        //        		tempa = CV_ACCELERATION_ADJUSTMENT;
        //    		else if ((packet->data[packet->data_start] & 0x0F) == 0x03)
        //          		tempa = CV_DECELERATION_ADJUSTMENT;
        //    		else
        //        		break; //can't act on it!
        //    		DCC_Config_Write_CV(tempa, packet->data[packet->data_start + 1]);
        //	    	break;
        case DCC_PACKET_OPS_MODE_LONG_CONFIG_KIND:
        case DCC_PACKET_OPS_MODE_LONG_CONFIG_BIT_MANIPULATION_KIND:
            if( (packet->kind == prev_packet_kind) && (packet_match == ADDRESS_MATCH) ) //only allow ops mode writes to primary address, not consist address
            {
                handleDirectModePacket(packet, 1);
            }
            break;
        //		case DCC_PACKET_SET_ADVANCED_ADDRESSING_KIND:
        //    		if (packet->data[packet->data_start] & 0x01)
        //        		DCC_Config_Write_CV(29, DCC_CV29 | 0x20);
        //    		else
        //        		DCC_Config_Write_CV(29, DCC_CV29 & ~0x20);
        //    		break;

        /**** Consisting Commands ****/
        case DCC_PACKET_CREATE_CONSIST_FACE_FORWARD_KIND:
        tempa = 0x80;
        //fall through
        case DCC_PACKET_CREATE_CONSIST_FACE_REVERSE_KIND:
        //count on tempa = 0 if we hit exactly here
        DCC_Config_Write_CV(CV_CONSIST_ADDRESS, packet->data[packet->data_start + 1] | tempa);
        break;
    }
    _time_of_last_packet = millis();
}

void Service_Mode_Process_Packet(DCC_Packet_t *packet)
{
    //this is tricky, for several reasons.
    // 1) page preload packets are going to determine how to interpret the service mode packets
    // 2) must receive two of the same kind in a row before allowed to act on it, so we have to keep track of that too.
    //first, compare packets!
    //	if (DCC_Packet_Equal(&prev_packet, packet))
    if (packet->kind == prev_packet_kind)
    {
        uint8_t CV = 0; //not a 10bit number, because we don't support CVs that high.
        uint8_t val;
        //uint8_t pos; //used by bit manip kind
        uint8_t service_mode_page = eeprom_read_byte((const uint8_t*) CV_PAGE); //used by service mode packet kind

        //same packet! We can act on it now.

        //for now, let's just keep it simple
        //uint8_t i = 1;
        switch (packet->kind & 0x0F) //bottom nibble indicates mode of operation
        {
            case SERVICE_MODE_DIRECT_KIND:
            case SERVICE_MODE_DIRECT_BIT_MANIPULATION_KIND:
                handleDirectModePacket(packet, 0);
                break;
            case SERVICE_MODE_PAGED_KIND:
                //really, four possibilities here: page read and write, and CV read and write
                switch (packet->data[0] & 0x07) //switch on last three bits
                {
                    case 0x00:
                    case 0x01:
                    case 0x02:
                    case 0x03: //these are the data registers, and are handled in the same way
                        CV = ((service_mode_page - 1) * 4) + (packet->data[0] & 0x07) + 1;
                        break;
                    case 0x04:
                        CV = 29; //ALWAYS!!!
                        break;
                    case 0x05: //paging register
                        //this is a special case.
                        CV = CV_PAGE;
                        break;
                    case 0x06: //version number, only on page 1, otherwise nothing
                        if (1 == service_mode_page)
                        CV = 7;
                        break;
                    case 0x07: //manufacturer ID, only on page 1, otherwise nothing
                        if (1 == service_mode_page)
                        CV = 8;
                        break;
                }
                if (CV != 0) //0 used to indicate no write
                {
                    val = packet->data[1];
                    //now check is this write or verify?
                    if (packet->data[0] & 0x08) //write
                    {
                        if(CV == CV_PAGE) 
                            eeprom_update_byte((const uint8_t*)CV_PAGE, val);
                        else
                            DCC_Config_Write_CV(CV, val);
                    }
                    else
                    {
                        DCC_Config_Verify_CV(CV, val);
                    }
                }
                break;
        }


        _time_of_last_packet = millis();
    }
    //now, copy the packet into prev_packet
    prev_packet_kind = packet->kind;
}

void DCC_Decoder_Update(void)
{
    //TODO handle timeout properly, if so configured.
    //TODO handle service mode

    //check timeouts
    if (DCC_SERVICE_MODE)
    {
        if (time_delta_32(millis(), _time_of_last_packet) > 20) //in service mode, time out after 20ms of no packets
        DCC_flags |= 0x02; //move to ops mode
    }
    else if (DCC_OPERATIONS_MODE)
    {
        uint8_t timeout = eeprom_read_byte((const uint8_t *) CV_PACKET_TIME_OUT_VALUE);
        if (timeout) // if 0, ignore timeout values
        {
            if (time_delta_32(millis(), _time_of_last_packet) >= (timeout * 1000)) //more than timeout seconds since we last received a packet of any kind.
            //TODO change this to a read from CV 11!
            {
                if (MOTOR_IS_FORWARD) //if forward
                Motor_EStop(1); //bring it to a halt
                else
                Motor_EStop(-1);
            }
        }
    }

    if (DCC_flags & DCC_FLAGS_READY)
    {
        uint8_t i;
        DCC_Packet_t packet;
        //packet.data_start = 0;
        cli();
        DCC_flags &= ~DCC_FLAGS_READY; //reset ready flag
        packet.size = DCC_rx_buffer[ready_buf].size;
        //DCC_rx_buffer[ready_buf].size = 0;
        for (i = 0; i < packet.size; ++i)
        {
            packet.data[i] = DCC_rx_buffer[ready_buf].data[i];
            //DCC_rx_buffer[ready_buf].data[i] = 0;
        }
		#ifndef __DEBUG //if debugging, don't set interrupts here, do it below.
        sei();
		#endif
		
        //this part should be fast enough to complete before the next packet is done transmitting
        //This is rather involved...lots of possibilities.
        if (!DCC_Packet_Check_CRC(&packet))
        {
            return; //packet is corrupted, ignore it.
        }

        DCC_Packet_Get_Kind(&packet);
        //We start in service mode by default. Determine if we need to transition to Ops mode
        //notice that an ops mode packet addressed to ANYONE is sufficient to trigger a transition
        if (!(packet.kind & SERVICE_MODE_KIND_MASK)) //we do it this way, because Reset packets evaluate as ops mode kinds, but do not trigger a transition
        {
            DCC_flags |= DCC_FLAGS_OPS_MODE_MASK;
        }

        //now, depending on mode, pick the right state machine
        if (DCC_OPERATIONS_MODE)
        Ops_Mode_Process_Packet(&packet);
        else
        Service_Mode_Process_Packet(&packet);
		
		#ifdef __DEBUG
		sei();
		#endif
    }
}