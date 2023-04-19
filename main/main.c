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

#define RPWM   16 
#define LPWM   18 
//#define RPWM_0 18
//#define RPWM_0 19 

#define A_ENC 20
#define B_ENC 21
#define PORTS_I2C_SDA 4
#define PORTS_I2C_SCL 5
/// TIME IN MS 
#define SAMPLING_TIME 1 ///TIME SAMPLING IN miliseconds 

#define FINAL_A 27
#define FINAL_B 28  

#define KP 0.0
#define KI 0.0 
#define KD 0.8 

//#define KP 2.0
//#define KI 5.2 
//#define KD 0.1 

static uint8_t clock_pwm = 0 ; 
static float sp_pwm ; 
static bool new_cmd ;  
uint8_t fifo_rx[BUFFER_RX]    ; 
uint8_t fifo_tx[BUFFER_TX]    ; 



bool systick(struct repeating_timer *t) ; 
void core1task(void) ; 
volatile uint16_t counter_test = 0 ; 


int main() {
    encoder_quad_t enc_test ; 
    BTS7960_t bridge_h = {
        RPWM,
        LPWM,
        0,
        0,
    } ;

    stdio_init_all() ; 
    printf("INICIO DE ROTADOR ROT_IAR \r\n") ;     
    init_pwm(&bridge_h)           ;        /// inicio de pwm 
    initPorts(A_ENC,B_ENC)        ;        /// inicio de encoders 
    init_switch(FINAL_A,FINAL_B)  ;        /// lectura de fin de carrera  
    bridge_h.percent_l = TOP_VALUE_COUNT ; 
    bridge_h.percent_r = 0               ;
    set_pwm(&bridge_h);
    while( isSwitchOn() != 1) ; 
    setZero() ; 
    bridge_h.percent_l = 0 ; 
    bridge_h.percent_r = TOP_VALUE_COUNT ;
    set_pwm(&bridge_h)   ;
    while( isSwitchOn() != 2) ; 
    /// 
    ///search a min pwm using for a clockwise 
    bridge_h.percent_l = TOP_VALUE_COUNT ; 
    bridge_h.percent_r = 0 ;
    set_pwm(&bridge_h)   ;
    sleep_ms(1000) ; 
    bridge_h.percent_l = 0 ; 
    bridge_h.percent_r = 0 ;
    set_pwm(&bridge_h)   ;
    sleep_ms(10) ; 
    getData(&enc_test) ;   
    int16_t angle_init  = enc_test.angle ; 
    uint16_t min_pwm_sa ; 
    uint16_t min_pwm_sh ; 
    bridge_h.percent_r = 0 ;  
    for (uint16_t i = 0; i<TOP_VALUE_COUNT;i++){
        bridge_h.percent_l = i ; 

        set_pwm(&bridge_h)   ;
        sleep_ms(100) ; 
        getData(&enc_test) ;   

        if (angle_init != enc_test.count_pulses){
            set_minsh(i) ; 
            break ; 
        }
    }
    sleep_ms(10) ; 
    bridge_h.percent_r = 0 ; 
    bridge_h.percent_l = 0 ; 
    set_pwm(&bridge_h)   ;
    sleep_ms(10) ; 
    getData(&enc_test) ;  
    angle_init = enc_test.count_pulses ; 
    for (uint16_t i = 0; i<TOP_VALUE_COUNT;i++){
        bridge_h.percent_r = i ; 
        set_pwm(&bridge_h)   ;
        sleep_ms(100) ; 
        getData(&enc_test) ;   
        if (angle_init != enc_test.count_pulses){
            set_minsa(i) ; 
            break ; 
        }
    }
    setttings_pid(KP,KD,KI) ; 
    multicore_launch_core1(core1task); ///START CORE1 
    fsm_init(0.001) ; 
    struct repeating_timer timer                                  ;
    add_repeating_timer_ms(SAMPLING_TIME,&systick, NULL, &timer ) ; 
    while (1) {
        if (new_cmd == true){
            new_cmd = false ; 
            cmd_receiveI2C(fifo_rx,BUFFER_RX) ; 
        }

        if (clock_pwm == 1){
           fsm_main_app() ; 
        }
        if (counter_test >= 1000){
            counter_test = 0 ; 
            getData(&enc_test) ;      
            printf("position is a : %0.2f \r\n",enc_test.angle )  ;
        }
    }
}


bool systick(struct repeating_timer *t) { 
    clock_pwm = 1 ; 
    counter_test++ ; 

    return true ; 
}


void dma_u1(uint8_t *bufferrx){
    ///buffer rx -> data received of dma channel!  
    memcpy(fifo_rx,bufferrx ,BUFFER_RX) ; 
    new_cmd=true ; 
}  

volatile void dma_u2(uint16_t *buffertx)
{
    encoder_quad_t enc ; 
    getData(&enc) ; 
    printf("angle is a %0.2f\r\n",enc.angle) ;
    buffertx[0] = (uint16_t) getState() ; 
    buffertx[1] = (uint16_t)((0xFF00 & ( (uint16_t) enc.angle))>>8) ; 
    buffertx[2] = (uint16_t)((0x00FF &  ((uint16_t) enc.angle))) ;
    buffertx[3] = (uint16_t)((enc.angle - (uint16_t) enc.angle )*100) ;  

}  


void core1task(void){ 
    init_I2C(4,5,dma_u1,dma_u2) ;

    while(1){
        tight_loop_contents();
    }


}