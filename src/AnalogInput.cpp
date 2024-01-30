#include "AnalogInput.h"
#include <cstdint>
#include "WProgram.h"
#include "Device.h"
#include "config.h"
#include "Const.h"

namespace tamproxy {

// Constructor takes a pin number
AnalogInput::AnalogInput(uint8_t pin) {
    _pin = pin;
    currentVoltage = 0;
    pinMode(pin, INPUT);
}

std::vector<uint8_t> AnalogInput::handleRequest(std::vector<uint8_t> &request) {
    if (request[0] == ANALOG_INPUT_READ_CODE) {
        if (request.size() != 1) return {REQUEST_LENGTH_INVALID_CODE};

        return {static_cast<uint8_t>(currentVoltage>>8), static_cast<uint8_t>(currentVoltage)};
    } else  {
        return {REQUEST_BODY_INVALID_CODE};
    }
}

void AnalogInput::doUpkeep() {
    // Read the voltage from the pin
    uint16_t measuredVoltage = analogRead(_pin);

    // Apply LP filter
    currentVoltage = LP_FILTER_ALPHA * currentVoltage + (1 - LP_FILTER_ALPHA) * measuredVoltage;
}

}
