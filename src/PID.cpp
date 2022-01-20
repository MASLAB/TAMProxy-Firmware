#include "PID.h"

PIDController::PIDController(float K, float Ti, float limit)
    : K(K)
    , Ti(Ti)
    , limit(limit)         // output supply limit     [volts]
    , error_prev(0.0f)
    , integral_prev(0.0f) 
{
    timestamp_prev = micros();
}

// PID controller function
float PIDController::operator() (float error){
    // calculate the time from the last call
    unsigned long timestamp_now = micros();
    float Ts = (timestamp_now - timestamp_prev) * 1e-6f;
    // quick fix for strange cases (micros overflow)
    if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

    // u(s) = (P + I/s + Ds)e(s)
    // Discrete implementations
    // proportional part
    // u_p  = P *e(k)
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

    // saving for the next pass
    integral_prev = integral;
    error_prev = error;
    return output;
}
