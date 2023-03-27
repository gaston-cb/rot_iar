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
 

bool isSwitchOn(){
    
    //if 
    ///routine of detected switch-case 
    return true ; 
} 