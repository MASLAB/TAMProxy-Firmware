#define LOOP_TIME 1000 //!< loop time in microseconds for controller
#define DEF_PID_POS_K 1.0f //!< default PID controller K value
#define DEF_PID_POS_TI 0.5f //!<  default PID controller Ti value
#define DEF_PID_POS_LIMIT 1.0f //!< default PID controller PWM limit

#define DEF_PID_VEL_K 0.5f
#define DEF_PID_VEL_TI 0.05f
#define DEF_PID_VEL_LIMIT 1.0f
#define DEF_VEL_FILTER_TF 0.001f //!< default filter time constant

#define LP_FILTER_ALPHA 0.98f

#define GEAR_RATIO 10.0f
#define CPR 64.0f // counts per revolution