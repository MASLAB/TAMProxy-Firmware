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
    _loopTime = LOOP_TIME; // [microseconds]
    pinMode(_motorPinA, OUTPUT);
    pinMode(_motorPinB, OUTPUT);
    pinMode(_motorPinPWM, OUTPUT);
    digitalWrite(_motorPinA, 0);
    digitalWrite(_motorPinB, 0);
    analogWrite(_motorPinPWM, 0);
    timestamp_prev = micros();
    desiredAngle = 0.0;
}

std::vector<uint8_t> FeedbackMotor::handleRequest(std::vector<uint8_t> &request) {
    unsigned long timestamp_now = micros();
    uint32_t elapsedTime = static_cast<uint32_t>(timestamp_now - timestamp_prev);
    if(elapsedTime > _loopTime)
    {
        timestamp_prev = timestamp_now;
        if (request[0] == FEEDBACK_MOTOR_WRITE_CODE) {
            if (request.size() != 4) return {REQUEST_LENGTH_INVALID_CODE};

            // desired angle is 16 bit
            desiredAngle = 2.0*_PI*((request[1]<<16) + (request[2]<<8) + request[3])/65536.0;
        }

        uint32_t raw_enc_val = static_cast<uint32_t>(_enc.read());
        float encoderAngle = 2.0*_PI*raw_enc_val/(CPR*GEAR_RATIO);

        float angleError = desiredAngle - encoderAngle;

        // control effort is constrained to be between -1.0 and 1.0
        float controlEffort = AngleController(angleError);

        // determine direction of spin
        if(controlEffort > 0)
        {
            digitalWrite(_motorPinA, 0);
            digitalWrite(_motorPinB, 1);
        }
        else
        {
            digitalWrite(_motorPinA, 1);
            digitalWrite(_motorPinB, 0);
        }

        // set 8-bit PWM to control speed; value between 0 and 255
        analogWrite(_motorPinPWM, static_cast<uint8_t>(255*_abs(controlEffort)));
    }
    if (request[0] == ENCODER_READ_CODE) {
        if (request.size() != 1) return {REQUEST_LENGTH_INVALID_CODE};

        // library is bad, should use unsigned for well-defined overflow
        uint32_t val = static_cast<uint32_t>(_enc.read());
        return {
            static_cast<uint8_t>(val>>24),
            static_cast<uint8_t>(val>>16),
            static_cast<uint8_t>(val>>8),
            static_cast<uint8_t>(val)
        };
    } else {
        return {REQUEST_BODY_INVALID_CODE};
    }
}
}
