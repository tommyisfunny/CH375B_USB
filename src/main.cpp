#include <Arduino.h>
#include <SoftwareSerial9.h>
#include <CH375B_API.hpp>

CH375_API ch375(4, 5, 2); // CH375 connected to pins 4 and 5 of Arduino Nano

void handleInterruptionState(uint8_t c);

void setup() {
  //ch375.setEventHandler(handleEvent);

  unsigned char res;
  Serial.begin(9600);

  /*** 5.4 CMD_RESET_ALL ***/
  Serial.println(F("\n## 5.4 CMD_RESET_ALL: Hard resetting CH375"));
  ch375.cmd_reset_all();

  /*** 5.1 CMD_GET_IC_VER ***/
  Serial.println(F("\n## 5.1 CMD_GET_IC_VER"));
  res = ch375.cmd_get_ic_ver();
  Serial.print(F("CH375B Version: 0x"));
  Serial.println(res - 0x80, HEX); // remove the bit 7 according to datasheet

  
  /*** 5.9 CMD_SET_USB_MODE ***/
  Serial.println(F("\n## 5.9 CMD_SET_USB_MODE: USB-HOST"));
  res = ch375.cmd_set_usb_mode(0x05);
  if (res == CMD_RET_SUCCESS) {
    Serial.println(F("SET_USB_MODE success"));
  }
  else {
    Serial.println(F("SET_USB_MODE failed"));
  }

  ch375.waitForConnect();

  /*** 5.10 CMD_TEST_CONNECT ***/
  Serial.println(F("\n## 5.10 CMD_TEST_CONNECT"));
  handleInterruptionState(ch375.cmd_test_connect());

  // GET Device speed
  Serial.println(F("\n## GET Device speed"));
  res = ch375.cmd_get_dev_rate();
  Serial.print(F("Device speed code: 0x"));
  Serial.println(res, HEX);
  if (res == 0x04) {
    Serial.println(F("Device speed: Low-speed"));
  }
  else {
    Serial.println(F("Device speed: Full-speed"));
  }

  // reset bus
  Serial.println(F("\n## Reset bus"));
  res = ch375.cmd_set_usb_mode(0x07);
  if (res == CMD_RET_SUCCESS) {
    Serial.println(F("SET_USB_MODE success"));
  }
  else {
    Serial.println(F("SET_USB_MODE failed"));
  }

  delay(1000); // wait for 1 second

  // set to USB host mode
  Serial.println(F("\n## Set to USB host mode"));
  res = ch375.cmd_set_usb_mode(0x06);
  if (res == CMD_RET_SUCCESS) {
    Serial.println(F("SET_USB_MODE success"));
  }
  else {
    Serial.println(F("SET_USB_MODE failed"));
  }
  
  ch375.waitForConnect();

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

  // Set address to 1
  Serial.println(F("\n## SET ADDRESS"));
  ch375.cmd(CMD_SET_ADDRESS);
  ch375.write(0x01); // set address to 1
  handleInterruptionState(ch375.waitForInterrupt());

  Serial.println(F("\n## SET_USB_ADDR"));
  ch375.cmd(CMD_SET_USB_ADDR);
  ch375.write(0x01); // set address to 1

  Serial.println("## get device descriptor");
  ch375.cmd(CMD_GET_DESCR);
  ch375.write(0x01);
  handleInterruptionState(ch375.waitForInterrupt());

  ch375.cmd(CMD_RD_USB_DATA0);
  uint8_t len = ch375.read();
  Serial.print(F("LEN: "));
  Serial.println(len);
  uint8_t *buf = new uint8_t[len];
  for(uint8_t i = 0; i < len; i++) {
    buf[i] = ch375.read();
    Serial.println(buf[i], HEX);
  }

  
  // get device descriptor
  //Serial.println(F("\n## GET DEVICE DESCRIPTOR"));
  //ch375.cmd(CMD_WR_USB_DATA7);
  //ch375.write(0x08); // data length
  //ch375.write(0x80); // bmRequestType: 0x80 (Device to Host, Standard, Device)
  //ch375.write(0x06); // bRequest: GET_DESCRIPTOR
  //ch375.write(0x00); // wValue: 0 (Descriptor Type: Device)


}

void loop() {
    delay(1000);
    handleInterruptionState(ch375.waitForInterrupt());
}

void handleInterruptionState(uint8_t c) {
  switch(c) {
    case USB_INT_SUCCESS:
      Serial.println(F("Operation successful"));
      break;
    case USB_INT_CONNECT:
      Serial.println(F("USB device connected"));
      break;
    case USB_INT_DISCONNECT:
      Serial.println(F("USB device disconnected"));
      break;
    case USB_INT_BUF_OVER:
      Serial.println(F("Buffer overflow"))  ;
      break;
    case USB_INT_DISK_READ:
      Serial.println(F("Reading from USB storage"));
      break;
    case USB_INT_DISK_WRITE:
      Serial.println(F("Writing to USB storage"));
      break;
    case USB_INT_DISK_ERR:
      Serial.println(F("USB storage error"));
      break;
    default:
      Serial.print(F("Error: operation failed with code 0x"));
      Serial.println(c, HEX);
      if((c & 0xF0) == 0x20) {
        Serial.println(F("Error: Host transmission error"));
        if (c & 0x10) Serial.println(F("If IN transaction, data packets are out of sync"));
        switch(c & 0x0F) {
          case 0xA:
            Serial.println(F("Device returned NAK"));
            break;
          case 0xE:
            Serial.println(F("Device returned STALL"));
            break;
          default:
            if ((c & 0x03) == 0) Serial.println(F("Device timed out"));
            else Serial.println(F("Device returned unknown error"));
        }
      }
      break;
  }
}
