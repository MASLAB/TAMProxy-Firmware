#include "FeedbackMotor.h"
#include <cstdint>
#include "WProgram.h"
#include "Device.h"
#include "config.h"

namespace tamproxy {

// Constructor takes a dir pin and a pwm output pin
FeedbackMotor::FeedbackMotor(uint8_t motorPinA, uint8_t motorPinB, uint8_t motorPinPWM, uint8_t encPinA, uint8_t encPinB) : _encPinA(encPinA), _encPinB(encPinB), _enc(encPinA, encPinB) {   
    _motorPinA = motorPinA;
    _motorPinB = motorPinB;
    _motorPinPWM = motorPinPWM;
    pinMode(_motorPinA, OUTPUT);
    pinMode(_motorPinB, OUTPUT);
    pinMode(_motorPinPWM, OUTPUT);
    digitalWrite(_motorPinA, 0);
    digitalWrite(_motorPinB, 0);
    analogWrite(_motorPinPWM, 0);
}

std::vector<uint8_t> FeedbackMotor::handleRequest(std::vector<uint8_t> &request) {
    if (request[0] == FEEDBACK_MOTOR_WRITE_CODE) {
        if (request.size() != 4) return {REQUEST_LENGTH_INVALID_CODE};

        // determine direction of spin
        if(request[1] != 0)
        {
            digitalWrite(_motorPinA, 0);
            digitalWrite(_motorPinB, 1);
        }
        else
        {
            digitalWrite(_motorPinA, 1);
            digitalWrite(_motorPinB, 0);
        }

        // set PWM to control speed
        analogWrite(_motorPinPWM, (request[2]<<8) + request[3]);
        return {OK_CODE};
    } else if (request[0] == ENCODER_READ_CODE) {
        if (request.size() != 1) return {REQUEST_LENGTH_INVALID_CODE};

        // library is bad, should use unsigned for well-defined overflow
        uint32_t val = static_cast<uint32_t>(_enc.read());
        return {
            static_cast<uint8_t>(val>>24),
            static_cast<uint8_t>(val>>16),
            static_cast<uint8_t>(val>>8),
            static_cast<uint8_t>(val)
        };
    } else if (request[0] == ENCODER_WRITE_CODE) {
        if (request.size() != 5) return {REQUEST_LENGTH_INVALID_CODE};

        uint32_t val = (request[1] << 24)
                     | (request[2] << 16)
                     | (request[1] << 8)
                     | (request[0] << 0);

        // again, should be unsigned, but we have to cast
        _enc.write(static_cast<int32_t>(val));
        return {OK_CODE};
    } else {
        return {REQUEST_BODY_INVALID_CODE};
    }

}
}
