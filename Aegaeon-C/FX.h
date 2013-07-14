/*
 * File:   FX.h
 * Author: dgoodman
 *
 * Created on October 22, 2012, 1:31 PM
 */

#ifndef __FX_H__
#define	__FX_H__

#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define FX_GROUP_1  0
#define FX_GROUP_2  1
#define FX_GROUP_3  2
#define FX_GROUP_4  3
#define FX_GROUP_5  4

#define FX_FLf         (1<<0)
#define FX_FLr         (1<<1)
#define FX_FL          (FX_FLf|FX_FLr)
#define FX_F1          (1<<2)
#define FX_F2          (1<<3)
#define FX_F3          (1<<4)
#define FX_F4          (1<<5)

#define FX_F5          (1<<0)
#define FX_F6          (1<<1)
#define FX_F7          (1<<2)
#define FX_F8          (1<<3)
#define FX_F9          (1<<4)
#define FX_F10         (1<<5)
#define FX_F11         (1<<6)
#define FX_F12         (1<<7)

#define FX_ON_FORWARD           (1<<0)
#define FX_ON_REVERSE           (1<<1)
#define FX_DIM_FORWARD_STOP     (1<<2)
#define FX_DIM_REVERSE_STOP     (1<<3)
#define FX_DIM_FORWARD          (1<<4)
#define FX_DIM_REVERSE          (1<<5)
#define FX_SOFT_START           (1<<6)
#define FX_RESERVED             (1<<7)

//non triggerable
#define FX_ANIMATION_NONE               0
#define FX_ANIMATION_STROBE             1
#define FX_ANIMATION_DOUBLE_STROBE      2
#define FX_ANIMATION_ROTARY             3
#define FX_ANIMATION_FIREBOX            4
#define FX_ANIMATION_LEFT_DITCH         5
#define FX_ANIMATION_RIGHT_DITCH        6
#define FX_ANIMATION_MARS               7
#define FX_ANIMATION_GYRA               8
#define FX_NUM_ANIMATIONS               9
#define FX_TRIGGERABLE_MASK				0x80
#define FX_ANIMATION_MASK				0x7F

#define FX_SOFT_START_THRESHOLD        220
#define FX_SOFT_START_PERIOD            10  //ms

void FX_Initialize(void);
void FX_Update(void);
void FX_SetFunction(uint8_t new_fx, uint8_t new_fx_group, uint8_t consisted); //, uint8_t new_fx_group);

#ifdef	__cplusplus
}
#endif

#endif	// __FX_H__
