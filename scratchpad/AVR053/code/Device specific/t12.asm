;*****************************************************************
;* - Description:  Device definition file for RC Calibration
;* - File:         t12.asm
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

.include "tn12def.inc"
.include "Common\memoryMap.inc"
.include "Device specific\t12_family_pinout.inc"

.equ	EEARH	= 0x30		;unused address in IO space
.equ	EEARL	= EEAR

.EQU OSC_VER = 1