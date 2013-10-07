/* 
 * File:   HardwareDefs.h
 * Author: dgoodman
 *
 * Created on October 5, 2013, 2:44 PM
 */

#ifndef HARDWAREDEFS_H
#define	HARDWAREDEFS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MOTOR_PWM_CONTROL       OCR0A
#define MOTOR_PWM_LEVEL(x)      (0xFF-x)  //motor outputs are 0xFF = 0V, 0x00 = 12V
#define MOTOR_PWM_LEVEL_GT(x)   (OCR0A < (0xFF-x))
#define MOTOR_PWM_LEVEL_LT(x)   (OCR0A > (0xFF-x))

#define MOTOR_ENABLED_STATE     ((OCR0A == 0xFF) && ((PORTB & (1 << PB1)) == 0x00) && ((PORTA & (1 << PA7)) == 0x00))
#define MOTOR_DEENERGIZED       (true)
#define MOTOR_ENERGERIZE        (false)
    
#define MOTOR_FWD_OUTPUT        (PORTB & (1 << PB1))
#define MOTOR_FWD_SET           (1 << PB1)
#define MOTOR_FWD_CLEAR         (0)
#define MOTOR_REV_OUTPUT        (PORTA & (1 << PA7))
#define MOTOR_REV_SET           (1 << PA7)
#define MOTOR_REV_CLEAR         (0)

#define OUTPUT1_OUTPUT          (PORTA & (1 << PORTA5))
#define OUTPUT1_ON              (1 << PORTA5)
#define OUTPUT1_OFF             (0)
#define OUTPUT2_OUTPUT          (PORTA & (1 << PORTA6))
#define OUTPUT2_ON              (1 << PORTA6)
#define OUTPUT2_OFF             (0)
    

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWAREDEFS_H */

