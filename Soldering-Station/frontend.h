#ifndef FRONTEND_H_
 #define FRONTEND_H_
/**********************************************************************************************************
                               CLASS SolderingStation_FE 
**********************************************************************************************************
 Function:    base class for generic frontends for soldering stations
 Input:       None
 Output:      None
 Discription: provides virtual functions as base class for common api
**********************************************************************************************************/

class SolderingStation_FE {

 public:

    SolderingStation_FE(){
    };
    virtual void display_welcome_logo( void );
    virtual void display_title( void );
    virtual void display_invert( bool invert );
    virtual void display_dim(bool dim );
    virtual void display_show_Temperatur(  uint16_t dispTemperature, uint8_t HeatPwr_Percent, uint16_t setpoint, fsmstate_t state, uint16_t timestamp );
    virtual void display_show_TempError( uint8_t ErrNo );
    virtual void display_show_Undervoltage(uint16_t Vin);
    virtual void display_show_sleep( uint16_t timestamp );

};

#endif
