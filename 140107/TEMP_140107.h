#ifdef _PLATFORM_HW_140107_ 
#ifndef TEMP_140107_H_
 #define TEMP_140107_H_ 
 
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
class TEMP_140107 {
    
public: 
    TEMP_140107(){};
    uint16_t Read( uint8_t avg_cnt);
    uint16_t GetLastValue( void ) ;
    
private:
    uint16_t LastReadTemp=999;

};
#endif
#endif
