#ifndef FEEDBACK_MOTOR_H
#define FEEDBACK_MOTOR_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "Encoder.h"
#include "PID.h"

// contains constants for control loops
#include "Const.h"

// A class that that controls motor controllers that take a dir and pwm inputs

namespace tamproxy {

class FeedbackMotor : public Device {
private:
    uint8_t _motorPinA;
    uint8_t _motorPinB;
    uint8_t _motorPinPWM;
    uint8_t _encPinA;
    uint8_t _encPinB;


    uint32_t _loopTime;
    float desiredAngle;

    ::Encoder _enc;
    volatile uint32_t _count;
    int32_t encCount;

    float K; //!< Loop gain 
    float Ti; //!< Integral time 
    float limit; //!< Maximum output value

    float error_prev; //!< last tracking error value
    float integral_prev; //!< last integral component value
    unsigned long timestamp_prev; //!< Last execution timestamp

public:
    FeedbackMotor(uint8_t aPin, uint8_t bPin, uint8_t pwmPin, uint8_t encPinA, uint8_t encPinB);
    std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
    void doUpkeep();
};

}
#endif