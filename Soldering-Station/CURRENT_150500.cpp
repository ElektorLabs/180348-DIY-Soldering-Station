#include "CURRENT_150500.h"

uint16_t CURRENT_150500::Read( uint8_t avg_cnt){
    
   
  uint32_t samples=0;
  float current=0;                                /* We need to provide the current global to the system */

                                 
      for(int i =0;i<avg_cnt;i++)                 //five ADC samples for accurate reading
      {
          samples += analogRead(A1);
      }
      samples = samples/ avg_cnt;
      
      /* seems we have 1V per Amp */
      //current = samples * (uint32_t)488 / (uint32_t)244 ;                   
      current = samples * 5;
      if(current < UINT16_MAX){
        LastReadCurrent = current;
      } else {
        LastReadCurrent = UINT16_MAX;
      }
  
  return LastReadCurrent;
}

uint16_t CURRENT_150500::GetLastValue( void ){
    return LastReadCurrent;
}
