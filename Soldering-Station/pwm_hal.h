/**********************************************************************************************************
                               CLASS SolderingStation_PWM 
**********************************************************************************************************
 Function:    base class for generic PWM for soldering stations
 Input:       None
 Output:      None
 Discription: provides virtual functions as base class for common api
**********************************************************************************************************/
class SolderingStation_PWM {

 public:
   SolderingStation_PWM( ){ };
   virtual void On( uint16_t value );
   virtual void Off( void );
   virtual uint16_t ReadPWMValue( void );

};