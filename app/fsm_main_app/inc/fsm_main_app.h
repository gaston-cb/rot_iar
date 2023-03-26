

typedef enum {
    WAITING  , 
    TRACKING ,
    ALARM    ,
}fsm_state_t ; 


void fsm_init() ; 
void fsm_change() ; 

void park() ; 
void readAngle() ; 
void controlPositions(void) ; 

