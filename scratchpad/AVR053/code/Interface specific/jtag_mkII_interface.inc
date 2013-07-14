;*****************************************************************
;*
;* - Description:  JTAGICE mkII interface definition file for 
;*                 RC Calibration
;*
;* - File:         jtag_interface.inc
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

.EQU	JTAG_CALIB = 1

;*****************************************************************
;*	Specify Calibration clock frequency
;*****************************************************************
.EQU	CALIB_CLOCK_FREQ	= 32653	;Calibration Clock frequency in Hz
									;16.00MHz/(2*245) CHANGE WITH CARE

;*****************************************************************
;*	Pin and port definitions
;*****************************************************************
.EQU	MISO 		= JTAG_CAL_MISO
.EQU	MOSI		= JTAG_CAL_MOSI

.EQU	CAL_PORT	= JTAG_CAL_PORT_REG
.EQU	CAL_DDR		= JTAG_CAL_DDR_REG
.EQU	CAL_PIN		= JTAG_CAL_PIN_REG

;*****************************************************************
;*	Macro used to disable and enable JTAG functions as these 
;*	override IO functions. Macro contains nop if not using JTAG.
;*****************************************************************
.MACRO	OPEN_INTERFACE_ACCESS
	ldi	@0, 1<<JTD
	out	MCUCSR, @0
	out	MCUCSR, @0
.ENDMACRO

.MACRO	CLOSE_INTERFACE_ACCESS
	ldi	@0, ~(1<<JTD)
	out	MCUCSR, @0
	out	MCUCSR, @0
.ENDMACRO

.MACRO	INTERFACE_BUGFIX
	nop
.ENDMACRO
