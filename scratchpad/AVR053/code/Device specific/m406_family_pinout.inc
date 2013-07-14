;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         m406_family_pinout.inc
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

.equ	JTAG_CAL_PORT_REG 	= PORTB
.equ	JTAG_CAL_DDR_REG 	= DDRB
.equ	JTAG_CAL_PIN_REG 	= PINB

.equ	JTAG_CAL_MOSI		= PB1	;TDI
.equ	JTAG_CAL_MISO		= PB0	;TDO
