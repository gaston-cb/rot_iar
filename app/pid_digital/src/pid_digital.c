#include "pwm_control.h" 
#include <stdio.h>

#include "pid_digital.h" 
#include "quadrature_encoders.h"



/*
 * 
 * constants_of_pid[0] -> constant proportional  (K_p)
 * constants_of_pid[1] -> constant derivative    (K_d)
 * constants_of_pid[2] -> constant integral      (K_i)
 * 
*/
static float constants_of_pid[3] ; 
static int _reference  = 90 ;  ///zenit_position 
static int error = 0 ; 
static int last_error = 0 ; 
static BTS7960_t pwm_motor ; 
static encoder_quad_t _encoder ; 



void setttings_pid(float proportional, float derivative, float integral){
    constants_of_pid[0] = proportional ; 
    constants_of_pid[1] = derivative   ; 
    constants_of_pid[2] = integral     ; 
    get_pwm(&pwm_motor) ; 
}


void compute_pid(float sp, float period_sample) {
    float error_integral  ; 
    float error_derivativo; 
    float error_constante ; 
    float signal_u  ; 
    _reference = sp ; 
    getData(&_encoder) ;     
    error = sp - _encoder.angle            ; 

    error_integral = (error + last_error)  ; 
    error_derivativo = (error - last_error)/period_sample ; 
    signal_u = error* constants_of_pid[0]+ 
               error_derivativo*constants_of_pid[1] +
               error_integral* constants_of_pid[2] ; 
    
    last_error = error ; 
    if (signal_u>0){
        pwm_motor.percent_h = (uint16_t) (signal_u*(TOP_VALUE_COUNT)/100.0 ) ; 
        pwm_motor.percent_l = 0 ; 
    }else if(signal_u == 0){
        pwm_motor.percent_l = 0 ; 
        pwm_motor.percent_h = 0 ; 
    }else if (signal_u<0){ 
        signal_u = -1.0*signal_u ;        
        pwm_motor.percent_h = 0 ;      
        pwm_motor.percent_l = (uint16_t) (  signal_u*(TOP_VALUE_COUNT)/100.0 ) ; 
    }
    set_pwm(&pwm_motor) ; 
}


