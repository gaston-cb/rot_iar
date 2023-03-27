

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



void fsm_init(float clock_sample) ; 
void fsm_main_app() ; 
/**
 * @brief Get the State object
 * 
 * @return w -> waiting 
 *         a -> alarm   
 *         t -> tracking  
 * 
 */ 
char getState() ; 


void park() ; 
void readAngle(float pos) ; 
void controlPositions(void) ; 
void cmd_receiveI2C(uint8_t *buffer_receive,size_t length)  ;  
