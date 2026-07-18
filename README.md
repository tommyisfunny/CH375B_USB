# _This is currently WIP_

# USB HOST Library for the CH375B module

This library should give you control over the CH375B's USB Host capabilities.
The end goal is to recieve keyboard input from an USB keyboard, but im not planning on creating a full HID class library.

Developed on an Arduino Nano (ATMega 328p)

[CH375B_API.hpp](https://github.com/tommyisfunny/CH375B_USB/blob/main/include/CH375B_API.hpp) : low level API for interfacing the CH375B. uses the parallel interface of the CH357B.
<br>
[CH375B_USB.hpp](https://github.com/tommyisfunny/CH375B_USB/blob/main/include/CH375B_USB.hpp) : handles USB stuff

## Datasheets

[CH375B datasheet part 1](https://www.wch-ic.com/downloads/CH375DS1_PDF.html)
<br>
[CH375B datasheet part 2](https://www.wch-ic.com/downloads/CH375DS2_PDF.html)
<br>
[USB 1.1 spec](http://esd.cs.ucr.edu/webres/usb11.pdf)
