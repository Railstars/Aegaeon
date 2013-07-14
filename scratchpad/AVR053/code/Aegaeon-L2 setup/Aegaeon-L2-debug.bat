cls
@echo off
	@ECHO *********************************************************
	@ECHO  Batch file for calibration of Atmel AVR Tiny84a
	@ECHO  oscillator through the ISP interface (STK500/AVRISP)
	@ECHO  - The internal RC is calibrated to value and accuracy 
	@ECHO    given in RC_Calibration.asm (fuses set for 8 MHz)
	@ECHO  - Programming FLASH and Fuses is performed initially.
	@ECHO  - stk500.exe -h / AVR Studio help for more options
	@ECHO  
	@ECHO  Revision: $Revision: 3.7 $                            
	@ECHO  Last Change: $Date: Thursday, August 19, 2004 13:06:46 UTC $
	@ECHO *********************************************************
	
	@ECHO ---------------------------------------------------------
	@ECHO.
	@ECHO   ** S T A R T   P R O G R A M M I N G **
	@ECHO.		
@ECHO ---------------------------------------------------------

"C:\Program Files (x86)\Atmel\AVR Tools\STK500\stk500.exe" -cUSB -I57600 -dattiny84a -s -fdfe2 -Eff -e -pf -if"..\rc_calib.hex"



@IF ERRORLEVEL ==1 GOTO prog_Calib_code_error
		
	@ECHO.
	@ECHO  ** Start Calibration

@ECHO ---------------------------------------------------------
"C:\Program Files (x86)\Atmel\AVR Tools\STK500\stk500.exe" -cUSB -dattiny84a -Y

@IF ERRORLEVEL ==1 GOTO Calibration_error

@ECHO ---------------------------------------------------------
	@ECHO.
	@ECHO Verify that OSCCAL value is differs from 0xFF.
	@ECHO Note, this test is intended to fail. If it does
	@ECHO not fail, OSCCAL equals 0xFF which is considered 
	@ECHO an error.

"C:\Program Files (x86)\Atmel\AVR Tools\STK500\stk500.exe" -cUSB -dattiny84a -ae0,0 -ve -ie0xFF_byte.hex


@IF ERRORLEVEL ==1 GOTO continue

@GOTO EEPROM_OSCCAL_value_error

:continue
		
@ECHO           ^^ Ignore Error above ^^
@ECHO.
@ECHO ---------------------------------------------------------
	@ECHO Read out new OSCCAL value from EEPROM and erase the device.
	@ECHO Program in new OSCCAL value in EEPROM at byte addr 01FF.
"C:\Program Files (x86)\Atmel\AVR Tools\STK500\stk500.exe" -cUSB -dattiny84a -e -fd5d2 -Eff -Z0 -Se0x01FF


@IF ERRORLEVEL ==1 GOTO prog_customer_code_error

@ECHO.
@ECHO ---------------------------------------------------------
	@ECHO Program in customers code to FLASH, in this case Aegaeon-L2.hex.
	@ECHO Verify programming of customers code.

"C:\Program Files (x86)\Atmel\AVR Tools\STK500\stk500.exe" -cUSB -dattiny84a -e -pf -vf -if"Aegaeon-L2.hex"


@IF ERRORLEVEL ==1 GOTO prog_customer_code_error

	@ECHO.
	@ECHO *********************************************************
	@ECHO 		P R O G R A M M I N G   O K
	@ECHO *********************************************************
	@PAUSE
	@GOTO END

:prog_Calib_code_error
	@ECHO.
@ECHO ---------------------------------------------------------
	@ECHO.
	@ECHO 		E R R O R		
	@ECHO Programming calibration program to AVR failed.
	@ECHO Programming aborted.
	@PAUSE
	@GOTO END
@ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


:Calibration_error
	@ECHO.
@ECHO ---------------------------------------------------------
	@ECHO.
	@ECHO 		E R R O R
	@ECHO Calibration failed.
	@ECHO Programming aborted.
	@PAUSE
	@GOTO END
@ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

:EEPROM_OSCCAL_value_error
	@ECHO.
@ECHO ---------------------------------------------------------
	@ECHO.
	@ECHO 		E R R O R
	@ECHO EEPROM OSCCAL location contain an invalid value: 0xFF
	@ECHO Programming aborted.
	@PAUSE
	@GOTO END
@ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

:prog_customer_code_error
	@ECHO.
@ECHO ---------------------------------------------------------
	@ECHO.
	@ECHO 		E R R O R
	@ECHO Programming main application program or OSCCAL to AVR failed.
	@ECHO Programming aborted.
	@PAUSE
	@GOTO END
@ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

:END				
