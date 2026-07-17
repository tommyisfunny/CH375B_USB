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
public:
    CH375B_USB(CH375B_API *api);

    bool init();
};

CH375B_USB::CH375B_USB(CH375B_API *api)
{
    this->api = api;
}

bool CH375B_USB::init() {
    uint8_t res;
    api->init();
    // reset chip
    api->cmd_reset_all();
    // set to usb host mode (no device)
    res = api->cmd_set_usb_mode(0x05);
    if (res != CMD_RET_SUCCESS) {
        DEBUGLNH(F("SET_USB_MODE failed"));
        return false;
    }
    
    return true;
}   
