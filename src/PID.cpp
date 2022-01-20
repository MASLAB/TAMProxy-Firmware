#include "PID.h"

PIDController::PIDController(float P, float I, float limit, float Ts)
    : P(P)
    , I(I)
    , limit(limit)         // output supply limit     [volts]
    , Ts(Ts)
    , error_prev(0.0f)
    , integral_prev(0.0f) 
{}

// PID controller function
float PIDController::operator() (float error){
    // u(s) = (P + I/s + Ds)e(s)
    // Discrete implementations
    // proportional part
    // u_p  = P *e(k)
    float proportional = P * error;
    // Tustin transform of the integral part
    // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
    float integral = integral_prev + I*Ts*0.5f*(error + error_prev);
    // antiwindup - limit the output
    integral = _constrain(integral, -limit, limit);
    
    // JOHNZ: implement derivative
    float derivative = 0.0;

    // sum all the components
    float output = proportional + integral + derivative;
    // antiwindup - limit the output variable
    output = _constrain(output, -limit, limit);

    // saving for the next pass
    integral_prev = integral;
    error_prev = error;
    return output;
}
