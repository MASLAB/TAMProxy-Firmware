#include "DFRMotor.h"
#include <cstdint>
#include "WProgram.h"
#include "Device.h"
#include "config.h"

namespace tamproxy {

// Constructor takes a dir pin and a pwm output pin
DFRMotor::DFRMotor(uint8_t aPin, uint8_t bPin, uint8_t pwmPin) {
    _aPin = aPin;
    _bPin = bPin;
    _pwmPin = pwmPin;
    pinMode(_aPin, OUTPUT);
    pinMode(_bPin, OUTPUT);
    pinMode(_pwmPin, OUTPUT);
    digitalWrite(_aPin, 0);
    digitalWrite(_bPin, 0);
    analogWrite(_pwmPin, 0);
}

std::vector<uint8_t> DFRMotor::handleRequest(std::vector<uint8_t> &request) {
    if (request[0] == MOTOR_WRITE_CODE) {
        if (request.size() != 4) return {REQUEST_LENGTH_INVALID_CODE};

        // determine direction of spin
        if(request[1] != 0)
        {
            digitalWrite(_aPin, 0);
            digitalWrite(_bPin, 1);
        }
        else
        {
            digitalWrite(_aPin, 1);
            digitalWrite(_bPin, 0);
        }

        // set PWM to control speed
        analogWrite(_pwmPin, (request[2]<<8) + request[3]);
        return {OK_CODE};
    } else {
        return {REQUEST_BODY_INVALID_CODE};
    }
}

}
