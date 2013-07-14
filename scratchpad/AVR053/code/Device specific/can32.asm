;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         can32.asm
;* - AppNote:      AVR053 - Production calibration of the
;*                          RC oscillator
;*
;* - Author:       Atmel Corporation: http://www.atmel.com
;*                 Support email: avr@atmel.com
;*
;* $Name$
;* $Revision: 61 $
;* $RCSfile$
;* $Date: 2006-03-23 11:14:13 +0100 (to, 23 mar 2006) $
;*****************************************************************

.include "can32def.inc"
.include "Common\memoryMap.inc"
.include "Device specific\m128_family_pinout.inc"

.equ OSC_VER	= 4

.equ	TCCR0	= TCCR0A
.equ	TIFR	= TIFR0
.equ	MCUCSR	= MCUCR
