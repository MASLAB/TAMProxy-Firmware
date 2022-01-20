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

    unsigned long timestamp_prev; //!< Last execution timestamp
public:
    FeedbackMotor(uint8_t aPin, uint8_t bPin, uint8_t pwmPin, uint8_t encPinA, uint8_t encPinB);
    PIDController AngleController{DEF_PID_POS_P, DEF_PID_POS_I, DEF_PID_POS_LIMIT, LOOP_TIME};
    std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
};

}
#endif