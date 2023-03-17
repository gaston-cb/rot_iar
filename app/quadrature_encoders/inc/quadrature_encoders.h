#include "pico/stdlib.h"
#define MAX_COUNT_PULSES_PER_REV 600  
#define CONVERSION_PULSES_FACTOR (360.0/600.0) //--> THIS A CONVERSION FACTOR USES FOR A TEST HW!!  
#define DELAY_SIGNAL 800 /// time signal for a control of still state encoder_t 


typedef enum{
    STATE_00 = 0  , //00
    STATE_01 = 1  , // 1   
    STATE_11 = 3  , // 3
    STATE_10 = 2  , // 2 

}state_quad_enc_t ; 

typedef enum{
    COUNTER_CLOCKWISE,
    COUNTER_ANTICLOCKWISE,
    COUNTER_STILL,  
}direction_t ;  


typedef struct{
    int16_t count_pulses ; //max 600 - min 0 
    state_quad_enc_t state ;
    direction_t direction ; 
    float speed ; 
    float angle ; 
}encoder_quad_t ; 








void initPorts(uint channel_a, uint channel_b) ; 
void setZero() ; 
void getData(encoder_quad_t *quadrature_enc)  ; 


