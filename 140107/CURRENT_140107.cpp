#ifdef _PLATFORM_HW_140107_ 
#include "CURRENT_140107.h"

uint16_t CURRENT_140107::Read( uint8_t avg_cnt, uint16_t current_PWM){
    
   
  uint32_t samples=0;
  float current=0;                                /* We need to provide the current global to the system */

  if( 1 == 0 ){                                   //Currently broken in HW
      for(int i =0;i<avg_cnt;i++)                 //five ADC samples for accurate reading
      {
          samples += analogRead(A1);
      }
      samples = samples/ avg_cnt;
     
      current = samples * (uint32_t)488 / (uint32_t)244 ;                   
       
      if(current < UINT16_MAX){
        LastReadCurrent = current;
      } else {
        LastReadCurrent = UINT16_MAX;
      }
  
  }      

  return LastReadCurrent;
}

uint16_t CURRENT_140107::GetLastValue( void ){
    return LastReadCurrent;
}
#endif