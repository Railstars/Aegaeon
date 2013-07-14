;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         m128_family_pinout.inc
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

.equ	ISP_CAL_PORT_REG	= PORTE
.equ	ISP_CAL_DDR_REG		= DDRE
.equ	ISP_CAL_PIN_REG		= PINE

.equ	ISP_CAL_MISO		= PE1       ; PDO
.equ	ISP_CAL_MOSI		= PE0       ; PDI

.equ	JTAG_CAL_PORT_REG 	= PORTF
.equ	JTAG_CAL_DDR_REG 	= DDRF
.equ	JTAG_CAL_PIN_REG 	= PINF

.equ	JTAG_CAL_MOSI		= PF7		;TDI
.equ	JTAG_CAL_MISO		= PF6		;TDO
