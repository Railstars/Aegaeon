;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         pwm2_family_pinout.inc
;* - AppNote:      AVR053 - Production calibration of the
;*                          RC oscillator
;*
;* - Author:       Atmel Corporation: http://www.atmel.com
;*                 Support email: avr@atmel.com
;*
;* $Name$
;* $Revision: 56 $
;* $RCSfile$
;* $Date: 2006-02-16 17:44:45 +0100 (to, 16 feb 2006) $
;*****************************************************************

.equ	ISP_CAL_PORT_REG	= PORTD
.equ	ISP_CAL_DDR_REG		= DDRD
.equ	ISP_CAL_PIN_REG		= PIND

.equ	ISP_CAL_MISO		=PORTD2
.equ	ISP_CAL_MOSI		=PORTD3

