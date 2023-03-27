/**
 * @file main.c
 * @authors gaston valdez - Dario Capucchio 
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * @copyright 
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "fsm_main_app.h"
#include <hardware/clocks.h>
#include <hardware/timer.h>
#include "quadrature_encoders.h"
#include "pwm_control.h"
#include "pid_digital.h"
#include "i2c_slave.h"

#define PORTS_PWM_L  16 
#define PORTS_PWM_L0 17 
#define PORTS_PWM_R  18
#define PORTS_PWM_R0 19 
#define PORTS_ENCODER_A 20
#define PORTS_ENCODER_B 21
#define PORTS_I2C_SDA 4
#define PORTS_I2C_SCL 5
/// TIME IN MS 
#define SAMPLING_TIME 1 


#define KP 1.0
#define KI 1.0 
#define KD 1.0 


static uint8_t clock_pwm = 0 ; 
static float sp_pwm ; 
static bool new_cmd ;  
uint8_t fifo_rx[BUFFER_RX]    ; 
uint8_t fifo_tx[BUFFER_TX]    ; 



bool systick(struct repeating_timer *t) ; 
void core1task(void) ; 



int main() {
    stdio_init_all() ; 
    
    BTS7960_t bridge_h = {
        PORTS_PWM_L,
        PORTS_PWM_R,
        0,
        0,
    } ;
    
    ///FIXME: SET ZERO AND SPIN THE MOTOR 
    // init_pwm(&bridge_h) ;
    // bridge_h.pwmh =MAX ; bridge_h.pwml = 0   
    // set_pwm(BTS7960_t *config_port )
    // WAIT_IRQ_GPIO_NORTH 
    // initPorts(PORTS_ENCODER_A,PORTS_ENCODER_B) ; 
    // setZero() ; 
    // setttings_pid(KP,KD,KI) ; 


    multicore_launch_core1(core1task); ///START CORE1 
    struct repeating_timer timer;
    add_repeating_timer_ms(SAMPLING_TIME,&systick, NULL, &timer ) ; 
    
    while (1) {
        if (new_cmd == true){
            new_cmd = false ; 
            //cmd_receiveI2C(uint8_t *buffer_receive) ; 
        }

        if (clock_pwm == 1){
           fsm_main_app() ; 
        }

    }
}


bool systick(struct repeating_timer *t) { 
    clock_pwm = 1 ; 
    return true ; 
}


void dma_u1(uint8_t *bufferrx){
    ///buffer rx -> data received of dma channel!  
    memcpy(fifo_rx,bufferrx ,BUFFER_RX) ; 
    new_cmd=true ; 
}  

void dma_u2(uint16_t *buffertx)
{
    uint16_t send_data[BUFFER_TX] ; 
    encoder_quad_t enc ; 
    getData(&enc) ; 
    send_data[0] = (uint8_t )getState() ; 
    memcpy(buffertx,send_data,BUFFER_TX) ; 
}  


void core1task(void){ 
    init_I2C(4,5,dma_u1,dma_u2) ;

    while(1){
        tight_loop_contents();

    }


}