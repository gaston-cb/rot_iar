#include "fsm_main_app.h" 


static fsm_main_app_t _state ; 
static float position ; 
event_t evCHANGE_STATE ; 



void fsm_init()  {
    _state = WAITING ; 
} 



void fsm_change() { 
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




void park() {

} 

void readAngle() {



}  
void controlPositions(void) {



}   