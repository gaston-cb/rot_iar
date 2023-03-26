

typedef enum {
    WAITING  , 
    TRACKING ,
    ALARM    ,
}fsm_main_app_t ; 

typedef enum {
    evTRACK,
    evWAIT,
    evALARM, 
    evNONE, ///AUX FOR A INIT STATE MACHINE 
}event_t ; 



void fsm_init() ; 
void fsm_change() ; 

void park() ; 
void readAngle() ; 
void controlPositions(void) ; 

