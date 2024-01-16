#ifndef PID_H
#define PID_H

#include "Utils.h"
#include "WProgram.h"

/**
 *  PID controller class
 */
class PIDController
{
public:
    /**
     *  
     * @param K - Loop gain 
     * @param Ti - Integral time
     * @param limit - Maximum output value
     */
    PIDController(float P, float I, float limit);
    ~PIDController() = default;
    
    float operator() (float error);

    float K; //!< Loop gain 
    float Ti; //!< Integral time 
    float limit; //!< Maximum output value

protected:
    float error_prev; //!< last tracking error value
    float integral_prev; //!< last integral component value
    float timestamp_prev; //!< last time stamp
};

#endif