#ifdef _PLATFORM_HW_140107_ 
#include "PWM_140107.h"

#include "TimerOne.h"
#define PWM_PIN ( 0 )
/**********************************************************************************************************
                                void On()        
**********************************************************************************************************
 Function:    void On()
 Input:       uint16_t value
 Output:      None
 Discription: Sets the PWM to the given value, is the value is zero, this turns the PWM off
**********************************************************************************************************/
void PWM_140107::On( uint16_t value ){
  
  if(value == 0)
	{
		Off();   //got some noise distortion on making OVR1B = 0 and hence turned timer off 
	}
	else
	{
    /*
   	 	TCCR1A |= (1 << COM1B1) |(1 << WGM11); //9bit fast PWM mode
		TCCR1B |= (1 << WGM12) | (1 << CS12);  //prescalar == 256 i.e. freq = 8/256 = 31.25KHz
		OCR1B = value ;
    */
    Timer1.setPwmDuty(PWM_PIN, value);
    
     
	}
}

/**********************************************************************************************************
                                void Off()        
**********************************************************************************************************
 Function:    void Off()
 Input:       None
 Output:      None
 Discription: Turns the PWM off
**********************************************************************************************************/
void PWM_140107::Off( void ) {

  Timer1.setPwmDuty(PWM_PIN, 0);
  /*
	TCCR1A = 0x00;
	TCCR1B = 0x00;
    OCR1B = 0;
  */

}  

/**********************************************************************************************************
                                uint16_t ReadPWMValue()        
**********************************************************************************************************
 Function:    uint16_t ReadPWMValue()     
 Input:       None
 Output:      uint16_t OCR3A
 Discription: Returns the RAW value ot the PWM
**********************************************************************************************************/  
uint16_t PWM_140107::ReadPWMValue( void ) {
    
    return OCR1B;
    
}

#endif
    
