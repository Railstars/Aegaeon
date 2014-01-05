Aegaeon
=======

A feature-rich, NMRA standards compliant DCC decoder built around the Atmel ATtiny84A.

This project comprises hardware, firmware, and documentation for the Railstars Aegaeon series of [NMRA DCC decoders](http://en.wikipedia.org/wiki/Digital_Command_Control).


The Intent
----------

Let's be honest: Most decoders manufactured in North America kinda suck. We can do better. I want to harness the power of open source development to put together the finest range of decoders available, anywhere. This project has multiple goals:
* NMRA standard conformance
* Robustness: decoders should be fault-tolerant, and compatible with widest range of other DCC equipment
* Size: Modern technology permits really tiny decoders
* Features: There is no excuse not to have the best feature set possible


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
* Trimming of internal RC oscillator is necessary for operation, requires Atmel AVRISP, a copy of stk500.exe, and MS DOS to calibrate. (Welcome to the 21st century!)


Firmware
--------

The firmware is written in pure C, and can be conditionally compiled for one of three possible configurations (motor only; motor + two functions; H-Bridge-driven function-only). Feature packed, and fully NMRA standards compliant, guaranteed by a stringent set of unit tests (in UnitTests folder, includes Xcode and NetBeans projects so you can run them straight on your Mac or PC).

### Features

* Momentum, 3-step and 28-step speed tables
* Button-controllable shunting mode
* Configurable function directionality, brightness, and Rule 17 dimming effects
* Configurable, button-controllable animations:
    * Strobe
    * Double strobe
    * Rotary beacon
    * Random flicker
    * Left/right ditch lights
    * Mars and Gyra lights
* Optional soft-start lighting effect
* Advanced consisting
* Supports Paged, Direct, Physical, and Operations mode programming
* Configurable DC operation, or stop-on-DC.


License
-------

The contents of the firmware folder is licensed under the GPLv3.
All other content is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported license


The Future
----------

There are a large number of features I would like to add to this product. I need your help!
- [ ] 2-layer PCB with easy-to-solder components for DIY
- [ ] ATmega328p/AT90CAN128 support for Arduino Uno/Railstars Io:duino
- [ ] Visual Studio project for unit tests
- [ ] Arduino project for easy integration into Arduino projects
- [ ] RailCom support (Railstars has a license to the necessary patents to implement RailCom, need to verify that implementing it as an open source project is kosher)
- [ ] On-the-tracks ops-mode bootloader. Ideally Arduino compatible but that might be a stretch. Would certainly require RailCom to work. But wouldn't that be awesome!?

Discussion
----------

You can get support for the Aegaeon software at the Railstars [Support Forum](http://support.railstars.com/index.php?p=/categories/aegaeon-firmware)

[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/Railstars/aegaeon/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

