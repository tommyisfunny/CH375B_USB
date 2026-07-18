#pragma once

#include <Arduino.h>
#include <CH375B_API.hpp>

#ifdef CH375B_USB_DEBUG
#define DEBUGLNH(x) Serial.print(F("CH375B_USB: "));Serial.println(x)
#define DEBUGH(x) Serial.print(F("CH375B_USB: "));Serial.print(x)
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#else
#define DEBUGLNH(x) ;
#define DEBUGH(x) ;
#define DEBUGLN(x) ;
#define DEBUG(x) ;
#endif

class CH375B_USB
{
private:
    CH375B_API *api;
    //static void ch357bEventCallback(uint8_t eventCode);

    void handleConnect();
    void handleDisconnect();

    bool resetBus();
    bool initDevice(bool lowspeed);
    bool readDeviceDescriptor();

    String getResponseString(uint8_t responseCode);
public:
    CH375B_USB(CH375B_API *api);

    bool init();
    void update();
};

CH375B_USB::CH375B_USB(CH375B_API *api)
{
    this->api = api;
}

bool CH375B_USB::init() {
    //api->setConnectionEventCallback(ch357bEventCallback);

    api->init();
    // reset chip
    api->cmd_reset_all();
    // set to usb host mode (no device)
    if(!api->cmd_set_usb_mode(USB_HOST_MODE_NO_DEVICE)) return false;

    
    return true;
}   

void CH375B_USB::update() {
    static uint32_t last = millis();
    uint32_t now = millis();
    static uint8_t lastConnectionState = USB_INT_DISCONNECT;

    if (now - last > 1000) {
        last = now;
        //DEBUGLNH(F("Checking for connect / disconnect of USB device"));
        uint8_t connectionState = api->cmd_test_connect();

        if (connectionState != lastConnectionState) {
            if      (connectionState == USB_INT_CONNECT)    handleConnect();
            else if (connectionState == USB_INT_DISCONNECT) handleDisconnect();
            lastConnectionState = connectionState;
        }
    }
}

void CH375B_USB::handleConnect() {
    DEBUGLNH(F("USB device connected"));
    // get the devices speed
    uint8_t res = api->cmd_get_dev_rate();
    bool lowspeed = res & 0x10;
    if (lowspeed) {DEBUGLNH(F("Device speed: Low-speed"));}
    else          {DEBUGLNH(F("Device speed: Full-speed"));}

    if(!initDevice(lowspeed)) {
        DEBUGLNH(F("Failed to initialize device"));
    }

}

bool CH375B_USB::initDevice(bool lowspeed = false) {
    // reset bus
    if (!resetBus()) return false;

    // set bus to slow speed if needed
    if(lowspeed) api->cmd_set_usb_speed(USB_SPEED_LOW);

    // wait for the device to reconnect
    api->waitForConnect();

    // Set the devices address to 1
    DEBUGLNH(F("Setting device address to 1"));
    if(!api->cmd_set_address(0x0C)) return false;

    api->cmd_set_usb_addr(0x0C);

    if (!readDeviceDescriptor()) return false;

    return true;
}

bool CH375B_USB::resetBus() {
    // reset bus
    DEBUGLNH(F("Reseting bus"));
    if(!api->cmd_set_usb_mode(USB_HOST_MODE_BUS_RESET)) return false;
  
    delay(10); 

    // set to USB host mode (auto SOF packages)
    if(!api->cmd_set_usb_mode(USB_HOST_MODE_NORMAL)) return false;
    return true;
}

bool CH375B_USB::readDeviceDescriptor() {
    DEBUGLNH(F("get device descriptor"));
    api->cmd(CMD_GET_DESCR);
    api->write(0x01);
    uint8_t result = api->waitForInterrupt();
    if (result != USB_INT_SUCCESS) {
        DEBUGH(F("Failed to get device descriptor: "));
        DEBUGLN(getResponseString(result));
        return false;
    }

    api->cmd(CMD_RD_USB_DATA0);
    uint8_t len = api->read();
    DEBUGH(F("LEN: "));
    DEBUGLN(len);
    uint8_t *buf = new uint8_t[len];
    for(uint8_t i = 0; i < len; i++) {
      buf[i] = api->read();
      DEBUGLN(buf[i]);
    }
    delete[] buf;
    return true;
}

void CH375B_USB::handleDisconnect() {
    DEBUGLNH(F("USB device disconnected"));

    // set to USB host mode (no device)
    api->cmd_set_usb_mode(USB_HOST_MODE_NO_DEVICE);
}

String CH375B_USB::getResponseString(uint8_t responseCode) {
    switch(responseCode) {
      case USB_INT_SUCCESS: return "Success";
      case USB_INT_CONNECT: return "Device connected";
      case USB_INT_DISCONNECT: return "Device disconnected";
      case USB_INT_BUF_OVER: return "Buffer overflow";
      case USB_INT_DISK_READ: return "Reading from USB storage";
      case USB_INT_DISK_WRITE: return "Writing to USB storage";
      case USB_INT_DISK_ERR: return "USB storage error";
      default:
        if((responseCode & 0xF0) == 0x20) {
          switch(responseCode & 0x0F) {
            case 0xA: return "Device returned NAK";
            case 0xE: return "Device returned STALL";
            default:
              if ((responseCode & 0x03) == 0) return "Device timed out";
              else return "Device returned unknown error";
          }
        }
        break;
    }
    return "code: " + String(responseCode, HEX);
}

//void CH375B_USB::ch357bEventCallback(uint8_t eventCode) {
    //if (eventCode == USB_INT_CONNECT) {
    //    DEBUGLNH(F("USB Device connected"));
    //} else {
    //    DEBUGLNH(F("USB Device disconnected"));
    //}
//}