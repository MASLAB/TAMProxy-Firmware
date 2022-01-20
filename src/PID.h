#ifndef PID_H
#define PID_H

#include "Utils.h"

/**
 *  PID controller class
 */
class PIDController
{
public:
    /**
     *  
     * @param P - Proportional gain 
     * @param I - Integral gain
     * @param limit - Maximum output value
     * @param Ts - Loop time
     */
    PIDController(float P, float I, float limit, float Ts);
    
    float operator() (float error);

    float P; //!< Proportional gain 
    float I; //!< Integral gain 
    float limit; //!< Maximum output value
    float Ts; //!< Loop time in microseconds

protected:
    float error_prev; //!< last tracking error value
    float integral_prev; //!< last integral component value
};

#endif