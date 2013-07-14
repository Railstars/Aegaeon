;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         t26.asm
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

.include "tn26def.inc"
.include "Common\memoryMap.inc"
.include "Device specific\t26_family_pinout.inc"

.EQU OSC_VER = 3

.equ	EEARL	= EEAR
.equ	EEARH	= 0x27		;Unused address in IO file