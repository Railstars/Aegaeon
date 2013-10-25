//
//  delay.c
//  UnitTests
//
//  Created by D.E. Goodman-Wilson on 10/24/13.
//  Copyright (c) 2013 D.E. Goodman-Wilson. All rights reserved.
//

#include <stdio.h>

uint16_t delay;

void _delay_loop_1(uint8_t __count)
{
    delay = 3*__count; //in cycles
}

void _delay_loop_2(uint16_t __count)
{
    delay = 4*__count; //in cycles
    //should probably advance _micros_counter?
}
