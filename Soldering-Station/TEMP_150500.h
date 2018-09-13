#ifndef TEMP_150500_H_
 #define TEMP_150500_H_ 
 
 /* includes for this class */
 #include "Arduino.h"
 #include "enums.h"

 


/* PIN definitions for the display */ 
#define OLED_CS        12
#define OLED_DC        14
#define OLED_CLK       15
#define OLED_DATA      16
#define OLED_RESET     3


/**********************************************************************************************************
                               CLASS FE_150500 
**********************************************************************************************************
 Baseclass:   SolderingStation_FE
 Function:    Derrived class for 150500 frontend
 Input:       None
 Output:      None
 Discription: Handels the frontend of 150500
**********************************************************************************************************/
class TEMP_150500 {
    
public: 
    TEMP_150500(){};
    uint16_t Read( uint8_t avg_cnt);
    uint16_t GetLastValue( void ) ;
    
private:
    uint16_t LastReadTemp=999;

};
#endif
