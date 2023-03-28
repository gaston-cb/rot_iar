#include "limit_switch.h"
#include "hardware/gpio.h"

static limit_switch_t limit_switch ; 

void init_switch(uint switch_port_a, uint switch_port_b){
    ///FIXME: ASSERT TO A PORT DIGITAL IF HAS ASSIGNED ERROR 
    gpio_init(switch_port_a)          ;
    gpio_set_dir(switch_port_a,false) ; 
    gpio_init(switch_port_b)          ;
    gpio_set_dir(switch_port_b,false) ; 
    limit_switch.limit_switch_port_a = switch_port_a ; 
    limit_switch.limit_switch_port_b = switch_port_b ; 
   
}
 
/**
 * @brief 
 * 
 * @return uint8_t 0: closed switch  
 *                 1: open switch port a 
 *                 2: open switch port b 
 */                 
uint8_t isSwitchOn(){

    uint8_t state_return = 0  ; 

    if ( gpio_get( limit_switch.limit_switch_port_a ) == LEVEL_DETECTED_CLOSE_SWITCH){

        state_return = 1 ; 

    } ; 

    if ( gpio_get( limit_switch.limit_switch_port_a ) == LEVEL_DETECTED_CLOSE_SWITCH){

        state_return = 2 ; 

    } ; 



    return state_return ; 

} 