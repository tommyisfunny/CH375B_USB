#pragma once

#include <Arduino.h>
#include <CH375B_API.hpp>

class CH375B_USB
{
private:
    CH375_API *api;
public:
    CH375B_USB(CH375_API *api);
    ~CH375B_USB();
};

CH375B_USB::CH375B_USB(CH375_API *api)
{
    this->api = api;
}

CH375B_USB::~CH375B_USB()
{
}
