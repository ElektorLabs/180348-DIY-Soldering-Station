#ifndef CURRENT_150500_H_
 #define CURRENT_150500_H_ 
 
 /* includes for this class */
 #include "Arduino.h"
 #include "enums.h"



/**********************************************************************************************************
                               CLASS FE_150500 
**********************************************************************************************************
 Baseclass:   SolderingStation_FE
 Function:    Derrived class for 150500 frontend
 Input:       None
 Output:      None
 Discription: Handels the frontend of 150500
**********************************************************************************************************/
class CURRENT_150500 {
    
public: 
    CURRENT_150500(){};
    uint16_t Read( uint8_t avg_cnt );
    uint16_t GetLastValue( void ) ;
    
private:
    uint16_t LastReadCurrent=0;

};
#endif
