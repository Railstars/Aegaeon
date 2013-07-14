;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         t13.asm
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

.include "tn13def.inc"
.include "Common\memoryMap.inc"

.EQU OSC_VER = 4

#if __AVRASM_VERSION__ < 2000
	.message "Not AVRASM v.2 or higher. Including redefines for PORTB0/PORTB1"
	.equ	PB0	= PORTB0
	.equ	PB1	= PORTB1
#endif


.include "Device specific\t12_family_pinout.inc"

.equ	SPH	= 0x12
.equ	EEARH	= 0x12

.equ	TCCR0	= TCCR0B
.equ	TIFR	= TIFR0
