;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         m48.asm
;* - AppNote:      AVR053 - Production calibration of the
;*                          RC oscillator
;*
;* - Author:       Atmel Corporation: http://www.atmel.com
;*                 Support email: avr@atmel.com
;*
;* $Name$
;* $Revision: 3901 $
;* $RCSfile$
;* $Date: 2008-04-30 14:31:21 +0200 (on, 30 apr 2008) $
;*****************************************************************

.include "m48def.inc"
.include "Common\memoryMap.inc"
.include "Device specific\m8_family_pinout.inc"

.EQU OSC_VER = 5

.equ	TCCR0	= TCCR0B
.equ	TIFR	= TIFR0
.equ 	EEARH	= 0x22


