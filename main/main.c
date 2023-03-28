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
#include "limit_switch.h"

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

#define PORT_SWITCH_0 14
#define PORT_SWITCH_1 15  

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
    
    init_pwm(&bridge_h) ;
    init_switch(PORT_SWITCH_0,PORT_SWITCH_1)  ; 
    bridge_h.percent_l =255 ; 
    bridge_h.percent_h = 0   ;
    set_pwm(&bridge_h);
    while( isSwitchOn() != 1) ; 
    setZero() ; 
    initPorts(PORTS_ENCODER_A,PORTS_ENCODER_B) ; 
    setttings_pid(KP,KD,KI) ; 
    multicore_launch_core1(core1task); ///START CORE1 
    struct repeating_timer timer;
    add_repeating_timer_ms(SAMPLING_TIME,&systick, NULL, &timer ) ; 
    fsm_init(0.001) ; 
    while (1) {
        if (new_cmd == true){
            new_cmd = false ; 
            cmd_receiveI2C(fifo_rx,BUFFER_RX) ; 
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
    ///enc.float convert to a byte for a send to master 
    send_data[0] = (uint8_t )getState() ; 
    send_data[1] =    (uint8_t) ((0xFF00 & ( (uint16_t) enc.angle))>>8) ; 
    send_data[2] =   (uint8_t)
                   ((0x00FF &  ((uint16_t) enc.angle))) ;
    send_data[3] =    (uint8_t)  ((enc.angle - (uint16_t) enc.angle )*100) ;  
    memcpy(buffertx,send_data,BUFFER_TX) ; 
}  


void core1task(void){ 
    init_I2C(4,5,dma_u1,dma_u2) ;

    while(1){
        tight_loop_contents();

    }


}