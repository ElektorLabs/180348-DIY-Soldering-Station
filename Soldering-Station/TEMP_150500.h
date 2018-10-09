#ifndef TEMP_150500_H_
 #define TEMP_150500_H_ 
 
 /* includes for this class */
 #include "Arduino.h"
 #include "enums.h"


/**********************************************************************************************************
                               CLASS TEMP_150500 
**********************************************************************************************************
 Baseclass:   N/A
 Function:    Reads the IRON Temperatur
 Input:       None
 Output:      None
 Discription: Handels the temperatur read of 150500
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
