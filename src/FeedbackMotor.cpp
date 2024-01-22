#include "FeedbackMotor.h"
#include <cstdint>
#include <math.h>
#include "WProgram.h"
#include "Device.h"
#include "config.h"

namespace tamproxy {

// Constructor takes a dir pin and a pwm output pin
FeedbackMotor::FeedbackMotor(uint8_t dirPin, uint8_t pwmPin, uint8_t encPinA, uint8_t encPinB, bool velControl) : _encPinA(encPinA), _encPinB(encPinB), _enc(encPinA, encPinB), error_prev(0.0f), integral_prev(0.0f), angle_prev(0.0f), count_prev(0) {   
    _dirPin = dirPin;
    _pwmPin = pwmPin;
    _loopTime = LOOP_TIME; // [microseconds]
    pinMode(_dirPin, OUTPUT);
    pinMode(_pwmPin, OUTPUT);
    analogWrite(_pwmPin, 0);
    timestamp_prev = micros();
    desiredAngle = 0.0;
    desiredVelocity = 0.0;
    velocityControl = velControl;
    if (velocityControl) {
        K = DEF_PID_VEL_K;
        Ti = DEF_PID_VEL_TI;
        limit = DEF_PID_VEL_LIMIT;
    } else {
        K = DEF_PID_POS_K;
        Ti = DEF_PID_POS_TI;
        limit = DEF_PID_POS_LIMIT;
    }
    Tf = DEF_VEL_FILTER_TF;
    encCount = 0;
    currentVelocity = 0.0;
}

std::vector<uint8_t> FeedbackMotor::handleRequest(std::vector<uint8_t> &request) {
    if (request[0] == FEEDBACK_MOTOR_WRITE_CODE) {
        if (request.size() != 5) return {REQUEST_LENGTH_INVALID_CODE};

        if (velocityControl)
        {
            float f;
            uint8_t b[] = {request[1], request[2], request[3], request[4]};
            memcpy(&f, &b, sizeof(f));
            desiredVelocity = f;
        } else { // Position Control
            float f;
            uint8_t b[] = {request[1], request[2], request[3], request[4]};
            memcpy(&f, &b, sizeof(f));
            desiredAngle = f;
        }
    }

    if (request[0] == ENCODER_READ_CODE) {
        if (request.size() != 1) return {REQUEST_LENGTH_INVALID_CODE};

        if (velocityControl) {
            // send current estimated velocity
            union {
                int32_t c;
                uint8_t bytes[4];
            } val;
            val.c = encCount;

            return {
                static_cast<uint8_t>(val.bytes[0]),
                static_cast<uint8_t>(val.bytes[1]),
                static_cast<uint8_t>(val.bytes[2]),
                static_cast<uint8_t>(val.bytes[3])
            };
        } else {
            // library is bad, should use unsigned for well-defined overflow
            uint32_t val = static_cast<uint32_t>(_enc.read());
            return {
                static_cast<uint8_t>(val>>24),
                static_cast<uint8_t>(val>>16),
                static_cast<uint8_t>(val>>8),
                static_cast<uint8_t>(val)
            };
        }
    } else {
        return {REQUEST_BODY_INVALID_CODE};
    }
}

void FeedbackMotor::doUpkeep() {
    unsigned long timestamp_now = micros();
    uint32_t elapsedTime = static_cast<uint32_t>(timestamp_now - timestamp_prev);
    if(elapsedTime > _loopTime)
    {
        float output = 0.0;
        float proportional = 0.0;
        float integral = 0.0;
        float error = 0.0;

        float Ts = elapsedTime*1.0e-6;

        uint32_t rawEncoderCount = static_cast<uint32_t>(_enc.read());
        encCount += int32_t(rawEncoderCount - encCount); // needed to handle int overflow correctly
        float currentAngle = 2.0*M_PI*encCount/(CPR*GEAR_RATIO);

        if (velocityControl) {
            // estimate velocity
            float estimatedVelocity = (currentAngle - angle_prev)/Ts;
            float alpha = 0.98;
            currentVelocity = alpha*currentVelocity + (1-alpha)*estimatedVelocity;

            error = desiredVelocity - currentVelocity;

            proportional = K * error;
            // Tustin transform of the integral part
            // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
            integral = integral_prev + K*Ts*0.5f/Ti*(error + error_prev);
            // antiwindup - limit the output
            integral = _constrain(integral, -limit, limit);

            // sum all the components
            output = proportional + integral;
            // antiwindup - limit the output variable
            output = _constrain(output, -limit, limit);

        } else { // position control
            error = M_PI/180.0*desiredAngle - currentAngle;

            proportional = K * error;
            // Tustin transform of the integral part
            // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
            integral = integral_prev + K*Ts*0.5f/Ti*(error + error_prev);
            // antiwindup - limit the output
            integral = _constrain(integral, -limit, limit);

            // sum all the components
            output = proportional + integral;
            // antiwindup - limit the output variable
            output = _constrain(output, -limit, limit);
        }

        // determine direction of spin
        if (output > 0)
        {
            digitalWrite(_dirPin, 0);
        }
        else
        {
            digitalWrite(_dirPin, 1);
        }

        
        // set 8-bit PWM to control speed; value between 0 and 255
        analogWrite(_pwmPin, static_cast<uint8_t>(255*fabs(output)));

        // saving for the next pass
        integral_prev = integral;
        error_prev = error;
        angle_prev = currentAngle;
        timestamp_prev = timestamp_now;
        count_prev = encCount;
    }
}
}
