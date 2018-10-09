#ifndef FRONTEND_150500_H_
 #define FRONTEND_150500_H_ 
 
 /* includes for this class */
 #include "Arduino.h"
 #include "enums.h"
 #include "Frontend.h"
 


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
class FE_150500 : public SolderingStation_FE {
    
public: 
    FE_150500(){}
    void display_setup( void );
    void display_welcome_logo( void );
    void display_title( void );
    void display_invert( bool invert );
    void display_dim(bool dim );
    void display_show_Temperatur(  uint16_t dispTemperature, uint8_t HeatPwr_Percent, uint16_t setpoint, fsmstate_t state, uint16_t timestamp );
    void display_show_TempError( uint8_t ErrNo );
    void display_show_Undervoltage(uint16_t Vin);
    void display_show_sleep( uint16_t timestamp );
};
#endif