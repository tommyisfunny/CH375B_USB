#include <Arduino.h>
#include <SoftwareSerial9.h>

#define CH375B_API_DEBUG
#include <CH375B_API.hpp>
#define CH375B_USB_DEBUG
#include <CH375B_USB.hpp>

CH375B_API ch375_api(4, 5, 2); // tx of CH375B, rx of CH375B, int pin
CH375B_USB ch375_usb(&ch375_api);

void handleInterruptionState(uint8_t c);

void setup() {
  Serial.begin(9600);

  ch375_usb.init();

}

void loop() {
  //delay(1000);
  //handleInterruptionState(ch375.waitForInterrupt());
  ch375_usb.update();
}



  // get device descriptor
  //Serial.println(F("\n## GET DEVICE DESCRIPTOR"));
  //ch375.cmd(CMD_WR_USB_DATA7);
  //ch375.write(0x08); // data length
  //ch375.write(0x80); // bmRequestType: 0x80 (Device to Host, Standard, Device)
  //ch375.write(0x06); // bRequest: GET_DESCRIPTOR
  //ch375.write(0x00); // wValue: 0 (Descriptor Type: Device)

  // send set address setup packet
  // Serial.println(F("\n## Send SET_ADDRESS setup packet"));
  // ch375.cmd(CMD_WR_USB_DATA7);
  // ch375.write(0x08); // data length
  // ch375.write(0x00); // bmRequestType: 0x00 (Host to Device, Standard, Device)
  // ch375.write(0x05); // bRequest: SET_ADDRESS
  // ch375.write(0x00); // wValue: 1 (new device address)
  // ch375.write(0x01); //
  // ch375.write(0x00); // wIndex: 0
  // ch375.write(0x00); //
  // ch375.write(0x00); // wLength: 0
  // ch375.write(0x00); //

  // ch375.cmd(CMD_ISSUE_TKN_X);
  // ch375.write(0x00); 
  // ch375.write(0x0D); // setup token to endpoint 0

  // handleInterruptionState(ch375.waitForInterrupt());

  // in 
  //ch375.cmd(CMD_ISSUE_TOKEN);
  ////ch375.write(0x00); 
  //ch375.write(0x09); // in token to endpoint 0
  //handleInterruptionState(ch375.waitForInterrupt());