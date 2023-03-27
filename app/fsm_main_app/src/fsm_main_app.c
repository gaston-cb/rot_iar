#include <pico/stdlib.h>
#include "fsm_main_app.h" 
#include "pid_digital.h"


static fsm_main_app_t _state ; 
static float position ; 
event_t evCHANGE_STATE ; 
static float clok_sample_ = 0 ; 


void fsm_init(float clock_init)  {

    _state = WAITING ; 
} 



void fsm_main_app() { 
    switch (_state)
    {
        case WAITING: 
            park() ; 
            break;
        case TRACKING:
            controlPositions() ; 
            break ; 
        case ALARM: 
            park() ; 
            break;
        default: ///FIXME: ASSERT TO A STATE NULL OR REINIT 
            break;
    }

}


char getState(){
    char st ; 
    switch (_state)
    {
    case WAITING: 
        st = 'w'  ; 
        break;
    case TRACKING:
        st = 't' ; 
        break ; 
    case ALARM: 
        st = 'a' ; 
        break ; 
    default:
        st = 'e' ; 
        break;
    }
    return st ; 
}

/**
 * @brief: set a change state for fsm main app  
 * 
 * @param _evrx:   evTRACK,
 *                 evWAIT,
 *                 evALARM,  
 */
void evHANDLER(event_t _evrx){ 
    fsm_main_app_t last_state = _state ;  
    switch (_evrx)
    {
        case evTRACK:
            

            break;
        case evWAIT:
            position= 90.0 ; 
            _state = WAITING ; 
            break;
        case evALARM:
            position= 90.0 ; 
            _state = ALARM ; 
            break;
        default:
            break;
    }
}




void park() {

    compute_pid(position, clok_sample_ ) ; 

} 


/**
 * @brief 
 * 
 */
void readAngle(float new_position) {
    position = new_position ; 


}  
void controlPositions(void) {
    compute_pid(position, clok_sample_ ) ;  // time in seconds unit     


}   


void cmd_receiveI2C(uint8_t *buffer_receive,size_t length) { 
    char rx_cmd =(char ) buffer_receive[0] ; 
    switch (rx_cmd)
    {
    case 'a':
        
        break;
    case 'w': 
        break ; 
    case 't': 
        break ; 
    case 'p': 
        break ; 
    default:
        break;
    }

}