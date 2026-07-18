#pragma once

#include <Arduino.h>
#include <CH375B_API.hpp>
#include <USB_structures.h>

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
    if(!api->cmd_get_descr(USB_DEVICE_DESCRIPTOR)) {
        DEBUGLNH(F("Failed to read device descriptor"));
    }

    USB_device_descriptor *desc = new USB_device_descriptor();


    api->cmd_rd_usb_data0((uint8_t*) desc, sizeof(*desc));

    DEBUGH(F("bcdUSB: "));
    DEBUGLN(String(desc->bcdUSB, HEX));
    DEBUGH(F("maxPacketSize: "));
    DEBUGLN(String(desc->bMaxPacketSize0));
    DEBUGH(F("VendorID: "));
    DEBUGLN(String(desc->idVendor, HEX));
    DEBUGH(F("PRoductID: "));
    DEBUGLN(String(desc->idProduct, HEX));

    delete[] desc;
    return true;
}

void CH375B_USB::handleDisconnect() {
    DEBUGLNH(F("USB device disconnected"));

    // set to USB host mode (no device)
    api->cmd_set_usb_mode(USB_HOST_MODE_NO_DEVICE);
}

//void CH375B_USB::ch357bEventCallback(uint8_t eventCode) {
    //if (eventCode == USB_INT_CONNECT) {
    //    DEBUGLNH(F("USB Device connected"));
    //} else {
    //    DEBUGLNH(F("USB Device disconnected"));
    //}
//}