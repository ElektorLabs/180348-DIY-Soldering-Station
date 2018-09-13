#include "CURRENT_150500.h"

uint16_t CURRENT_150500::Read( uint8_t avg_cnt, uint16_t current_PWM){
    
   
  uint16_t samples_A0=0;
  float current=0;                                /* We need to provide the current global to the system */
  float volt_adc;                                 //voltage reading for calculation of current 

  if( 1 == 0 ){                                   //Currently broken in HW
      for(int i =0;i<avg_cnt;i++)                 //five ADC samples for accurate reading
      {
          samples_A0 += analogRead(A0);
      }
      samples_A0 = samples_A0/ avg_cnt;
      volt_adc = map(samples_A0,0,1023,0,5000);
      current = volt_adc*(current_PWM/512.0); ;                   
       
      if(current < UINT16_MAX){
        LastReadCurrent = current;
      } else {
        LastReadCurrent = UINT16_MAX;
      }
  
  }      

  return LastReadCurrent;
}

uint16_t CURRENT_150500::GetLastValue( void ){
    return LastReadCurrent;
}