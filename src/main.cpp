#include <Arduino.h>
//#define CH375B_API_DEBUG
#include <CH375B_API.hpp>
#define CH375B_USB_DEBUG
#include <CH375B_USB.hpp>

const uint8_t cs = A4;
const uint8_t rd = A1;
const uint8_t wr = A2;
const uint8_t a0 = A0;
const uint8_t rst = A3;
const uint8_t intPin = 2;

uint8_t dataPins[8] = {12, 11, 10, 9, 8, 7, 6, 5};

CH375B_API ch375_api(cs, rd, wr, a0, rst, intPin, dataPins);
CH375B_USB ch375_usb(&ch375_api);

void setup() {
    Serial.begin(9600);
    delay(2000);

    ch375_usb.init();
}

void loop() {
    ch375_usb.update();
}
