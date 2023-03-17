#include "quadrature_encoders.h"
#include <stdio.h> 
#include "pico/stdlib.h" 
#include <string.h>
#include "hardware/clocks.h"
#include "hardware/timer.h"


// internal functions ! 
static void gpio_callback_channel_ab(uint pin, uint32_t mask) ; 
static void fsm_encoder(const state_quad_enc_t new_state) ; 
bool alarma(struct repeating_timer *t) ; 
static void read_state() ; 


/// @brief internal variables- > method is oo for C 
static  encoder_quad_t encoder ;
static volatile uint8_t count_state  = 0  ; 
static volatile uint16_t port_chanel_a = 0 ; 
static volatile uint16_t port_chanel_b = 0; 
static volatile uint64_t clock_speed ; 

bool alarma(struct repeating_timer *t){
    printf("getting started\r\n") ; 
//    read_state() ; 
    return true  ; 
}

void initPorts(uint port_chanel_a, uint port_chanel_b){
    ///FIXME: manejar errores de canales a,b asociados a puertos 
    printf("iNICIO DE PORTS \r\n")    ; 
    port_chanel_a = port_chanel_a     ; 
    port_chanel_b = port_chanel_b     ; 
    gpio_init(port_chanel_a)          ;
    gpio_set_dir(port_chanel_a,false) ; 
    gpio_pull_up(port_chanel_a) ; 
    gpio_init(port_chanel_b)  ;
    gpio_set_dir(port_chanel_b,false) ; 
    gpio_pull_up(port_chanel_b) ;     
    gpio_set_irq_enabled(port_chanel_a,GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE  ,true) ; 
    gpio_set_irq_enabled(port_chanel_b,GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE  ,true) ; 
    gpio_set_irq_callback(&gpio_callback_channel_ab);     
    printf("iNICIO DE PORTS_end \r\n") ; 

    irq_set_enabled(IO_IRQ_BANK0, true);
    struct repeating_timer timer0;
    bool ms_al =  add_repeating_timer_ms((int32_t) 800,&alarma, NULL, &timer0 ) ; 
    printf("init alamr is %s\r\n", ms_al == true ?"true":"false") ; 
} 
 





void read_state(){ 
    // 0 -> low , !=0 for high --> extract api docs ! 
    uint8_t new_state   =   ((uint8_t)(gpio_get(port_chanel_a)<<1) |  gpio_get(port_chanel_b))   ; 
    encoder.direction = (new_state == encoder.state)?COUNTER_STILL:encoder.direction;  
    count_state = 0 ; 
}



void setZero(){
    encoder.angle = 0 ; 
    encoder.count_pulses = 0 ; 
    encoder.state = ((uint8_t)(gpio_get(port_chanel_a)<<1) |  gpio_get(port_chanel_b))   ;
    encoder.direction = COUNTER_STILL ; 
}  



static void fsm_encoder(const state_quad_enc_t new_state){
    switch(encoder.state){
        case STATE_00:           
            if (new_state == STATE_10){
                encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed = time_us_64() - clock_speed ; 
                    count_state =  0 ; 
                    encoder.count_pulses++ ; 
                    encoder.speed = (1/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed =  time_us_64()  ; 
                }
            }else if (new_state == STATE_01){
                encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    clock_speed =  time_us_64()  ; 

                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                }

                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed = time_us_64() - clock_speed ;                     
                    encoder.count_pulses-- ; 
                    encoder.speed = -1.0*(1/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 

                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    count_state = 0; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 

                }
            }
            break ; 
        case STATE_10:
            if (new_state == STATE_11){
                 encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed = time_us_64() - clock_speed ; 
                    count_state = 0 ; 
                    encoder.count_pulses++ ; 
                    encoder.speed = (1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 

                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }
            }else if (new_state == STATE_00){
                encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    clock_speed = time_us_64() ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ;  
                }
                count_state++ ; 
                if (count_state == 4){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = -(1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 

                    count_state = 0  ; 
                    encoder.count_pulses-- ; 
                    encoder.speed = 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }

            }

            break ; 
        case STATE_11:
            if (new_state == STATE_01){
                encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state == 4){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = (1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    count_state = 0 ; 
                    encoder.count_pulses++ ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }
            }else if (new_state == STATE_10){
                 encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ;  
                }

                count_state++ ; 
                if (count_state == 4){ 
                    clock_speed = time_us_64() - clock_speed ; 
                    count_state = 0 ; 
                    encoder.count_pulses-- ; 
                    encoder.speed = -(1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }
            }
            break ; 
        case STATE_01:
            if (new_state == STATE_00){
                 encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed =time_us_64() - clock_speed ; 
                    encoder.speed = (1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    count_state =0 ; 
                    encoder.count_pulses++ ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 

                }
            }else if (new_state == STATE_11){
                encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ;  
                }

                count_state++ ; 
                if (count_state%4 == 0){
                    count_state = 0 ; 
                    clock_speed = time_us_64() - clock_speed ;  
                    clock_speed = time_us_64() ;
                    encoder.speed = -(1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    encoder.count_pulses-- ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses *CONVERSION_PULSES_FACTOR ; 
                }

            }
            break ; 
        default:  //safe code 
            break ; 
    }    
}





static void gpio_callback_channel_ab(uint gpio,uint32_t event_mask ) { 
    uint8_t new_state ; 
    
    
    if (gpio == port_chanel_a){
        new_state = gpio_get(port_chanel_b) ; 
        if (event_mask == GPIO_IRQ_EDGE_FALL){ 
        ///edge fall -> channel a 1 -> 0
            new_state = 0<<1 | new_state; 
        }else if(event_mask == GPIO_IRQ_EDGE_RISE) {
            ///edge rise -> channel a 0 -> 1 
            new_state = 1<<1 | new_state; 
        }

    }else if (gpio == port_chanel_b){
        new_state =  gpio_get(port_chanel_a)<<1 ; 
        if (event_mask == GPIO_IRQ_EDGE_FALL){ 
            new_state = new_state | 0 ; 

        }else if(event_mask == GPIO_IRQ_EDGE_RISE) {        
            new_state = new_state | 1 ; 
        }


    }else return ; 
    
    fsm_encoder(new_state);  
}



void getData(encoder_quad_t *quadrature_enc) {
    memcpy(quadrature_enc ,&encoder ,sizeof(encoder_quad_t)) ; 
} 





