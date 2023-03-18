/**
 * @file main.c
 * @authors gaston valdez - Dario Capucchio 
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * @copyright 
 */

#include <stdio.h>
#include <hardware/clocks.h>
#include <hardware/timer.h>

#include "pico/stdio.h"
#include "quadrature_encoders.h"
#include "pwm_control.h"
#include "pid_digital.h"
#define PORTS_PWM_L  16 
#define PORTS_PWM_L0 17 
#define PORTS_PWM_R  18
#define PORTS_PWM_R0 19 
#define PORTS_ENCODER_A 20
#define PORTS_ENCODER_B 21
#define PORTS_I2C_SDA 4
#define PORTS_I2C_SCL 5
#define SAMPLING_TIME 1 /// TIME IN MS 


#define KP 1.0
#define KI 1.0 
#define KD 1.0 



bool systick(struct repeating_timer *t) ; 

int main() {
    stdio_init_all() ; 
    BTS7960_t bridge_h = {
        PORTS_PWM_L,
        PORTS_PWM_R,
        0,
        0,
    } ;
    ///FIXME: SET ZERO AND SPIN THE MOTOR 
    init_pwm(&bridge_h) ; 
    setttings_pid(KP,KD,KI) ; 
    setZero() ; 
    initPorts(PORTS_ENCODER_A,PORTS_ENCODER_B) ; 
    sleep_ms(3000) ;
    struct repeating_timer timer;
    add_repeating_timer_ms(SAMPLING_TIME,&systick, NULL, &timer ) ; 
    encoder_quad_t enc ; 

    while (1) {
        sleep_ms(1000)  ; 
        ///! isr_start == true -> fsm_state 

    }
}


bool systick(struct repeating_timer *t) { 
    compute_pid(90.0, 0.001) ;     
    return true ; 

}