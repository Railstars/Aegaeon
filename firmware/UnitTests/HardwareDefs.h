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
    
//                  PB0 (fwd)   PB1 (rev)   PA7 (fpwm)  PB2 (rpwm)
//FORWARD =         HIGH        LOW         HIGH        LOW
//REVERSE =         LOW         HIGH        LOW         HIGH
//BRAKE   =         HIGH        HIGH        LOW         LOW
//  or              LOW         LOW         HIGH        HIGH
//COAST   =         LOW         LOW         LOW         LOW
//I think.

#define MOTOR_PWM_CONTROL       (OCR0A | OCR0B)
#define MOTOR_PWM_LEVEL(x)      (x)  //motor outputs are 0xFF = 0V, 0x00 = 12V
#define MOTOR_PWM_LEVEL_GT(x)   (MOTOR_PWM_CONTROL > (x))
#define MOTOR_PWM_LEVEL_LT(x)   (MOTOR_PWM_CONTROL < (x))

#define MOTOR_ENABLED_STATE     ( ((PORTB & (1 << PB0)) == (1 << PB0)) && ((PORTA & (1 << PA7)) == (1 << PA7))  \
                              ||  ((PORTB & (1 << PB1)) == (1 << PB1)) && ((PORTB & (1 << PB2)) == (1 << PB2)) )
#define MOTOR_DEENERGIZED       (false)
#define MOTOR_ENERGERIZED       (true)
    
//TODO need to incorporate a test for this AND use it in conjunction with the above to ensure against overshoot states when energizing motor
#define MOTOR_SHOOTTHRU_STATE   ( ((PORTB & (1 << PB0)) == (1 << PB0)) && ((PORTB & (1 << PB2)) == (1 << PB2)) \
                              ||  ((PORTB & (1 << PB1)) == (1 << PB1)) && ((PORTA & (1 << PA7)) == (1 << PA7)) )
#define MOTOR_SHOTTHRU_NONE     (false)

#define MOTOR_BRAKE_STATE       ( ((PORTB & (1 << PB0)) == (1 << PB0)) && ((PORTB & (1 << PB1)) == (1 << PB1)) \
                               && ((PORTA & (1 << PA7)) == 0) && ((PORTB & (1 << PB2)) == 0) )
#define MOTOR_BRAKE_SET         (true)
#define MOTOR_BRAKE_NOTSET      (false)
    
//TODO need to incorporate these;
#define MOTOR_COAST_STATE       ( ((PORTB & (1 << PB0)) == 0) && ((PORTB & (1 << PB1)) == 0) \
                               && ((PORTA & (1 << PA7)) == 0) && ((PORTB & (1 << PB2)) == 0) )
#define MOTOR_COAST_SET         (true)
#define MOTOR_COAST_UNSET       (false)

#define MOTOR_FWD_OUTPUT        (PORTB & (1 << PB0))
#define MOTOR_FWD_SET           (1 << PB0)
#define MOTOR_FWD_CLEAR         (0)
#define MOTOR_REV_OUTPUT        (PORTB & (1 << PB1))
#define MOTOR_REV_SET           (1 << PB1)
#define MOTOR_REV_CLEAR         (0)

#define MOTOR_DIRECTION         ((PORTB & (1 << PB0)) | (PORTB & (1 << PB1))) //HACK but it works
#define MOTOR_FORWARD           (1 << PB0)
#define MOTOR_REVERSE           (1 << PB1)
    
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

