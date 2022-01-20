#ifndef FEEDBACK_MOTOR_H
#define FEEDBACK_MOTOR_H

#include <cstdint>
#include <vector>
#include "Device.h"

// A class that that controls motor controllers that take a dir and pwm inputs

namespace tamproxy {

class FeedbackMotor : public Device {
private:
    uint8_t _aPin;
    uint8_t _bPin;
    uint8_t _pwmPin;
public:
    FeedbackMotor(uint8_t aPin, uint8_t bPin, uint8_t pwmPin);
    std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
};

}
#endif