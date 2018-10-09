#include "PWM_150500.h"


/**********************************************************************************************************
                                void On()        
**********************************************************************************************************
 Function:    void On()
 Input:       uint16_t value
 Output:      None
 Discription: Sets the PWM to the given value, is the value is zero, this turns the PWM off
**********************************************************************************************************/
void PWM_150500::On( uint16_t value ){
     if(value ==0)
    {
      Off();
    }else
    {
    
      DDRC  |= (1<< PWM_PIN);
      TCCR3A = (1 << COM3A1) | (1 << WGM31);
      TCCR3B = (1 << CS31) | (1 << WGM32) |(1 << CS30);  // 9 bit Fast PWM with 64 prescaling
      OCR3A  = value;                                    //adjust PWM asper temperature calculations
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
void PWM_150500::Off( void ) {

    TCCR3A  = 0;
    TCCR3B  = 0;
    OCR3A   = 0; 

}  

/**********************************************************************************************************
                                uint16_t ReadPWMValue()        
**********************************************************************************************************
 Function:    uint16_t ReadPWMValue()     
 Input:       None
 Output:      uint16_t OCR3A
 Discription: Returns the RAW value ot the PWM
**********************************************************************************************************/  
uint16_t PWM_150500::ReadPWMValue( void ) {
    
    return OCR3A;
    
}

    
    
