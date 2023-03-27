#include "fsm_main_app.h" 
#include "pid_digital.h"

static fsm_main_app_t _state ; 
static float position ; 
event_t evCHANGE_STATE ; 
static float clok_sample_ = 0 ; 


void fsm_init()  {
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
    switch (_evrx)
    {
        case evTRACK:
            _state = TRACKING ; 
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

} 


/**
 * @brief 
 * 
 */
void readAngle(float new_position) {
    position = new_position ; 


}  
void controlPositions(void) {
    // compute_pid(sp, period_clk ) // time in seconds unit     


}   