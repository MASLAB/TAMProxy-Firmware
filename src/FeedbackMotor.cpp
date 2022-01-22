#include "FeedbackMotor.h"
#include <cstdint>
#include <math.h>
#include "WProgram.h"
#include "Device.h"
#include "config.h"

namespace tamproxy {

// Constructor takes a dir pin and a pwm output pin
FeedbackMotor::FeedbackMotor(uint8_t motorPinA, uint8_t motorPinB, uint8_t motorPinPWM, uint8_t encPinA, uint8_t encPinB) : _encPinA(encPinA), _encPinB(encPinB), _enc(encPinA, encPinB), error_prev(0.0f), integral_prev(0.0f) {   
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
    K = DEF_PID_POS_K;
    Ti = DEF_PID_POS_TI;
    limit = DEF_PID_POS_LIMIT;
    encCount = 0;
}

std::vector<uint8_t> FeedbackMotor::handleRequest(std::vector<uint8_t> &request) {
    if (request[0] == FEEDBACK_MOTOR_WRITE_CODE) {
        if (request.size() != 5) return {REQUEST_LENGTH_INVALID_CODE};
            if (request[1] != 0)
                desiredAngle = 2*M_PI*((request[2]<<16) + (request[3]<<8) + request[4])/255;
            else
                desiredAngle = -2*M_PI*((request[2]<<16) + (request[3]<<8) + request[4])/255;
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

void FeedbackMotor::doUpkeep() {
    unsigned long timestamp_now = micros();
    uint32_t elapsedTime = static_cast<uint32_t>(timestamp_now - timestamp_prev);
    if(elapsedTime > _loopTime)
    {
        float Ts = elapsedTime*1.0e-6;
        timestamp_prev = timestamp_now;

        uint32_t rawEncoderCount = static_cast<uint32_t>(_enc.read());
        encCount = int32_t(rawEncoderCount - encCount);

        float encoderAngle = 2.0*M_PI*encCount/(CPR*GEAR_RATIO);
        
        float error = desiredAngle - encoderAngle;

        float proportional = K * error;
        // Tustin transform of the integral part
        // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
        float integral = integral_prev + K*Ts*0.5f/Ti*(error + error_prev);
        // antiwindup - limit the output
        integral = _constrain(integral, -limit, limit);

        // sum all the components
        float output = proportional + integral;
        // antiwindup - limit the output variable
        output = _constrain(output, -limit, limit);

        // determine direction of spin
        if(output > 0)
        {
            digitalWrite(_motorPinA, 0);
            digitalWrite(_motorPinB, 1);
        }
        else if(output < 0)
        {
            digitalWrite(_motorPinA, 1);
            digitalWrite(_motorPinB, 0);
        }
        else
        {
            digitalWrite(_motorPinA, 1);
            digitalWrite(_motorPinB, 1);
        }

        // set 8-bit PWM to control speed; value between 0 and 255
        analogWrite(_motorPinPWM, static_cast<uint8_t>(255*fabs(output)));

        // saving for the next pass
        integral_prev = integral;
        error_prev = error;
    }
}
}
