#pragma once

#include <Arduino.h>
#include "SoftwareSerial9.h"
// commands
#define CMD_GET_IC_VER    0x01
#define CMD_SET_BAUDRATE  0x02
#define CMD_ENTER_SLEEP   0x03
#define CMD_SET_USB_SPEED 0x04
#define CMD_RESET_ALL     0x05
#define CMD_CHECK_EXIST   0x06
#define CMD_GET_MAX_LUN   0x0A
#define CMD_GET_DEV_RATE  0x0A
#define CMD_SET_DISK_LUN  0x0B
#define CMD_SET_RETRY     0x0B
#define CMD_DELAY_100US   0x0F
#define CMD_SET_USB_ADDR  0x13
#define CMD_SET_USB_MODE  0x15
#define CMD_TEST_CONNECT  0x16
#define CMD_ABORT_NAK     0x17
#define CMD_SET_ENDP6     0x1C
#define CMD_SET_ENDP7     0x1D
#define CMD_GET_STATUS    0x22
#define CMD_RD_USB_DATA0  0x27
#define CMD_RD_USB_DATA   0x28
#define CMD_WR_USB_DATA7  0x2B
#define CMD_CLR_STALL     0x41
#define CMD_SET_ADDRESS   0x45
#define CMD_GET_DESCR     0x46
#define CMD_SET_CONFIG    0x49
#define CMD_AUTO_SETUP    0x4D
#define CMD_ISSUE_TKN_X   0x4E
#define CMD_ISSUE_TOKEN   0x4F
#define CMD_DISK_BOC_CMD  0x50
#define CMD_DISK_INIT     0x51
#define CMD_DISK_RESET    0x52
#define CMD_DISK_SIZE     0x53
#define CMD_DISK_READ     0x54
#define CMD_DISK_RD_GO    0x55
#define CMD_DISK_WRITE    0x56
#define CMD_DISK_WR_GO    0x57
#define CMD_DISK_INQUIRY  0x58
#define CMD_DISK_READY    0x59
#define CMD_DISK_R_SENSE  0x5A
#define CMD_DISK_MAX_LUN  0x5D
// operation states
#define CMD_RET_SUCCESS 0x51
#define CMD_RET_ABORT   0x5F
// interruption states
#define USB_INT_SUCCESS    0x14
#define USB_INT_CONNECT    0x15
#define USB_INT_DISCONNECT 0x16
#define USB_INT_BUF_OVER   0x17
#define USB_INT_DISK_READ  0x1D
#define USB_INT_DISK_WRITE 0x1E
#define USB_INT_DISK_ERR   0x1F

struct CH375_API {
  static CH375_API *instance;
  SoftwareSerial9 *port;
    
  volatile uint8_t status;
  volatile bool interruptFlag;
  static void isr();

  uint8_t intPin;

  CH375_API(uint8_t s1, uint8_t s2, uint8_t intPin = 3);

  unsigned char read();
  void write(uint16_t c);
  void cmd(uint16_t c);
  uint8_t getInterruptState();
  uint8_t waitForInterrupt();

  void waitForConnect();

  uint8_t cmd_test_connect();
  uint8_t cmd_get_ic_ver();
  void cmd_reset_all();
  uint8_t cmd_set_usb_mode(uint8_t mode);
  uint8_t cmd_get_dev_rate();
};

CH375_API *CH375_API::instance = nullptr;

CH375_API::CH375_API(uint8_t s1, uint8_t s2, uint8_t intPin) {
  if (instance != nullptr) {
    Serial.println(F("Error: Only one instance of CH375 is allowed."));
    return;
  }
  instance = this;
  port = new SoftwareSerial9(s1, s2, false);
  port->begin(9600);
  this->intPin = intPin;
  pinMode(intPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPin), isr, FALLING);
}

unsigned char CH375_API::read() {
    if(port->available()) {
      uint8_t c = port->read();
      return c & 0xFF;
    }
    return 0;
}

void CH375_API::write(uint16_t c) {
  port->write9(c);
  delay(1);
}

void CH375_API::cmd(uint16_t c) {
  write(c + 0x100); // bit 9 = 1
}

uint8_t CH375_API::getInterruptState() {
  interruptFlag = false;
  cmd(CMD_GET_STATUS);
  return read();
}

uint8_t CH375_API::waitForInterrupt() {
  uint32_t startTime = millis();
  while(!interruptFlag) {
    delay(1);
  }
  uint32_t endTime = millis();
  Serial.print("Interrupt detected after ");
  Serial.print(endTime - startTime);
  Serial.println(" milliseconds.");
  return getInterruptState();
}


void CH375_API::isr() {
  CH375_API::instance->interruptFlag = true;
}

void CH375_API::waitForConnect() {
  Serial.println(F("Waiting for USB device to connect..."));
  uint8_t interruptState = waitForInterrupt();
  while (interruptState != USB_INT_CONNECT) {
    Serial.print(F("."));
    delay(1000);
    interruptState = waitForInterrupt();
  }
  Serial.println(F("\nUSB device connected!"));
}

uint8_t CH375_API::cmd_test_connect() {
  cmd(CMD_TEST_CONNECT);
  return read();
}

uint8_t CH375_API::cmd_get_ic_ver() {
  cmd(CMD_GET_IC_VER);
  return read();
}

void CH375_API::cmd_reset_all() {
  cmd(CMD_RESET_ALL);
  delay(200); // wait until reset is done
}

uint8_t CH375_API::cmd_set_usb_mode(uint8_t mode) {
  cmd(CMD_SET_USB_MODE);
  write(mode);
  delay(1);
  return read();
}

uint8_t CH375_API::cmd_get_dev_rate() {
  cmd(CMD_GET_DEV_RATE);
  write(0x07); 
  return read();
}