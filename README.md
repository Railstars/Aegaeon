Aegaeon
=======

A feature-rich, NMRA standards compliant DCC decoder built around the Atmel ATtiny84A.

This project comprises hardware, firmware, and documentation for the Railstars Aegaeon series of [NMRA DCC decoders](http://en.wikipedia.org/wiki/Digital_Command_Control).

Hardware
--------

The hardware features an Atmel ATtiny84A microcontroller, a high-current H-Bridge with BEMF feedback, and two high-current open-collector outputs for lighting effects.

### Features

* Ultra-compact size (approx. 11mm x 6.7mm)
* 1A motor outputs
* Feedback motor control (aka BEMF)
* 16KHz or 32KHz motor operating modes
* Two high-current (300mA) function outputs
* Debugging header for use with DEBUGwire devices like Atmel JTAGICE mk2


Firmware
--------

The firmware is written in pure C, and can be conditionally compiled for one of three possible configurations (motor only; motor + two functions; H-Bridge-driven function-only). Feature packed, and fully NMRA standards compliant, guaranteed by a stringent set of unit tests (not yet uploaded, but to be available soon).

### Features

* Momentum, 3-step and 28-step speed tables
* Button-controllable shunting mode
* Configurable function directionality, brightness, and Rule 17 dimming effects
* Configurable, button-controllable animations:
* * Strobe
* * Double strobe
* * Rotary beacon
* * Random flicker
* * Left/right ditch lights
* * Mars and Gyra lights
* Optional soft-start lighting effect
* Advanced consisting
* Supports Paged, Direct, Physical, and Operations mode programming
* Configurable DC operation, or stop-on-DC.


License
-------

The contents of the firmware folder is licensed under the GPLv3.
All other content is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported license
