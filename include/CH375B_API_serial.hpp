#pragma once

#ifdef CH375B_API_DEBUG
#define DEBUGLNH(x) Serial.print(F("CH375B_API: "));Serial.println(x)
#define DEBUGH(x) Serial.print(F("CH375B_API: "));Serial.print(x)
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#else
#define DEBUGLNH(x) delay(10);
#define DEBUGH(x) delay(10);
#define DEBUGLN(x) delay(10);
#define DEBUG(x) delay(10);
#endif

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

class CH375B_API {
  static CH375B_API *instance;
  SoftwareSerial9 *port;
    
  volatile uint8_t status;
  volatile bool interruptFlag;
  static void isr();
  void (*connectionEventCallback)(uint8_t eventCode);

  uint8_t intPin;

  public:
  // tx of CH375B, rx of CH375B, int pin
  CH375B_API(uint8_t tx, uint8_t rx, uint8_t intPin); 

  bool init();

  void setConnectionEventCallback(void (*callback)(uint8_t));

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
  void cmd_set_address(uint8_t address);
  void cmd_set_usb_addr(uint8_t address);
};

CH375B_API *CH375B_API::instance = nullptr;

CH375B_API::CH375B_API(uint8_t tx, uint8_t rx, uint8_t intPin) {
  if (instance != nullptr) {
    DEBUGLNH(F("Error: Only one instance of CH375 is allowed."));
    return;
  }
  instance = this;
  port = new SoftwareSerial9(tx, rx, false);
  this->intPin = intPin;
  this->connectionEventCallback = nullptr;
}

bool CH375B_API::init() {
  port->begin(9600);
  pinMode(intPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPin), isr, FALLING);
  // get version
  uint8_t res = cmd_get_ic_ver();
  DEBUGH(F("CH375B Version: 0x"));
  DEBUGLN(res); 
  return true;
}

void CH375B_API::setConnectionEventCallback(void (*callback)(uint8_t)) {
  connectionEventCallback = callback;
}

unsigned char CH375B_API::read() {
  if(port->available()) {
    uint8_t c = port->read();
    return c & 0xFF;
  }
  return 0;
}

void CH375B_API::write(uint16_t c) {
  port->write9(c);
  delay(1);
}

void CH375B_API::cmd(uint16_t c) {
  write(c + 0x100); // bit 9 = 1
}

uint8_t CH375B_API::getInterruptState() {
  interruptFlag = false;
  cmd(CMD_GET_STATUS);
  uint8_t res = read();
  return res;
}

uint8_t CH375B_API::waitForInterrupt() {
  uint32_t startTime = millis();
  while(!interruptFlag) {
    delay(1);
  }
  uint32_t endTime = millis();
  //DEBUGH("Interrupt detected after ");
  //DEBUG(endTime - startTime);
  //DEBUGLN(" milliseconds.");
  uint8_t res = getInterruptState();
  delay(10);
  return res;
}


void CH375B_API::isr() {
  //CH375B_API::instance->cmd(CMD_GET_STATUS);
  //uint8_t eventCode = CH375B_API::instance->read();
  //uint8_t eventCode = CH375B_API::instance->getInterruptState();
  CH375B_API::instance->interruptFlag = true;
  //if(CH375B_API::instance->connectionEventCallback != nullptr && 
  //  (eventCode == USB_INT_CONNECT || eventCode == USB_INT_DISCONNECT)) {
  //  //CH375B_API::instance->connectionEventCallback(eventCode);
  //}
}

void CH375B_API::waitForConnect() {
  DEBUGLNH(F("Waiting for USB device to connect... "));
  uint8_t interruptState = waitForInterrupt();
  while (interruptState != USB_INT_CONNECT) {
    DEBUG(F("X"));
    delay(1000);
    interruptState = waitForInterrupt();
  }
  DEBUG("\n");
  DEBUGLNH(F("USB device connected!"));
}

uint8_t CH375B_API::cmd_test_connect() {
  DEBUGLNH(F("CMD_TEST_CONNECT"));
  cmd(CMD_TEST_CONNECT);
  delay(1);
  return read();
}

uint8_t CH375B_API::cmd_get_ic_ver() {
  DEBUGLNH(F("CMD_GET_IC_VER"));
  cmd(CMD_GET_IC_VER);
  return read()- 0x80; // remove the bit 7 according to datasheet
}

void CH375B_API::cmd_reset_all() {
  DEBUGLNH(F("CMD_RESET_ALL"));
  cmd(CMD_RESET_ALL);
  delay(200); // wait until reset is done
}

uint8_t CH375B_API::cmd_set_usb_mode(uint8_t mode) {
  DEBUGLNH(F("CMD_SET_USB_MODE"));
  cmd(CMD_SET_USB_MODE);
  write(mode);
  delay(1);
  return read();
}

uint8_t CH375B_API::cmd_get_dev_rate() {
  DEBUGLNH(F("CMD_GET_DEV_RATE"));
  cmd(CMD_GET_DEV_RATE);
  write(0x07); 
  return read();
}

void CH375B_API::cmd_set_address(uint8_t address) {
  DEBUGLNH(F("CMD_SET_ADDRESS"));
  cmd(CMD_SET_ADDRESS);
  write(address);
}


void CH375B_API::cmd_set_usb_addr(uint8_t address) {
  DEBUGLNH(F("CMD_SET_USB_ADDR"));
  cmd(CMD_SET_USB_ADDR);
  write(address);
}