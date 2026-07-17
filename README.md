# _This is currently WIP_

# USB HOST Library for the CH357B module

This library should give you control over the CH357B's USB Host capabilitys.
The end goal is to recieve keyboard input from an USB keyboard, but im not planning on creating a full HID class library.

Developed on a Arduino Nano (ATMega 328p)

[CH357B_API.hpp](https://github.com/tommyisfunny/CH375B_USB/blob/main/include/CH375B_API.hpp) : low level API for interfacing the CH375B. uses software serial for comunication
<br>
[CH357B_USB.hpp](https://github.com/tommyisfunny/CH375B_USB/blob/main/include/CH375B_USB.hpp) : handles USB stuff

## Wiring

| Arduino | CH375B |
|---------|--------|
| 5V      | 5V     |
| GND     | GND    |
| D5      | RXD    |
| D4      | TXD    |
| D2      | INT    |

## Datasheets

[CH357B datasheet part 1](https://www.wch-ic.com/downloads/CH375DS1_PDF.html)
<br>
[CH357B datasheet part 2](https://www.wch-ic.com/downloads/CH375DS2_PDF.html)
<br>
[USB 1.1 spec](http://esd.cs.ucr.edu/webres/usb11.pdf)