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
#define MOTOR_ENABLE_OUTPUT     (PORTB & (1 << PB2))
#define MOTOR_ENABLE_SET        (1 << PB2)
#define MOTOR_ENABLE_CLEAR      (0)
#define MOTOR_FWD_OUTPUT        (PORTB & (1 << PB1);
#define MOTOR_FWD_SET           (1 << PB1)
#define MOTOR_FWD_CLEAR         (0)
#define MOTOR_REV_OUTPUT        (PORTA & (1 << PA7);
#define MOTOR_REV_SET           (1 << PA7)
#define MOTOR_REV_CLEAR         (0)


#ifdef	__cplusplus
}
#endif

#endif	/* HARDWAREDEFS_H */

