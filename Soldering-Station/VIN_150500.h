#ifndef VIN_150500_H_
 #define VIN_150500_H_ 
 
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
class VIN_150500 {
    
public: 
    VIN_150500(){};
    uint16_t Read( uint8_t avg_cnt);
    uint16_t GetLastValue( void ) ;
    
private:
    uint16_t LastReadVIN=0;

};
#endif