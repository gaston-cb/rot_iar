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
static float error = 0 ; 
static float last_error = 0 ; 
static BTS7960_t pwm_motor ; 
static encoder_quad_t _encoder ; 



void setttings_pid(float proportional, float derivative, float integral){
    constants_of_pid[0] = proportional ; 
    constants_of_pid[1] = derivative   ; 
    constants_of_pid[2] = integral     ; 
    get_pwm(&pwm_motor) ; 
}


void compute_pid(float sp, float period_sample) {
    int error_integral  ; 
    int error_derivativo; 
    int error_constante ; 
    int signal_u  ; 
    _reference = sp ; 
    getData(&_encoder) ;     
     error = sp - _encoder.angle            ; 
    //last_error = error  ;
    error = (error<0)?-1*error:error ; 
    error_integral = (error + last_error)  ; 
    error_derivativo = (error - last_error)/period_sample ; 
    signal_u = error* constants_of_pid[0]+ 
               error_derivativo*constants_of_pid[1] +
               error_integral* constants_of_pid[2] ; 
    
    last_error = error  ; 
    ///ver si esta trabado el encoder ! 
    if ( sp - _encoder.angle>0){
        pwm_motor.percent_h = (uint16_t) (signal_u*(TOP_VALUE_COUNT)/100.0 ) ; 
        pwm_motor.percent_l = 0 ; 
    }else if( sp - _encoder.angle == 0){
        pwm_motor.percent_l = 0 ; 
        pwm_motor.percent_h = 0 ; 
    }else if ( sp - _encoder.angle <0){ 
        error = -1.0*error ;        
        pwm_motor.percent_h = 0 ;      
        pwm_motor.percent_l = (uint16_t) (  signal_u*(TOP_VALUE_COUNT)/100.0 ) ; 
    }
    set_pwm(&pwm_motor) ; 
}


