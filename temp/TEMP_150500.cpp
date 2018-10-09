#include "TEMP_150500.h"

/**********************************************************************************************************
                                uint16_t Read( uint8_t )        
**********************************************************************************************************
 Function:    uint16_t Read( uint8_t )     
 Input:       uint16_t avg_cnt
 Output:      uint16_t Temp in °C
 Discription: Reads the Temp in °C
**********************************************************************************************************/
uint16_t TEMP_150500::Read( uint8_t avg_cnt){
    
    uint16_t temperature=999;
    uint16_t samples=0;
    uint16_t ADCvoltage =0;  
    for(uint8_t i =0;i< avg_cnt;i++)
    {
      samples += analogRead(A0);
    } 
    samples = samples / avg_cnt;
    ADCvoltage = map(samples,0,1023,0,5000);

   if(ADCvoltage>2000){
      temperature=999;
   } else if( (ADCvoltage>=1550) && (ADCvoltage < 2000) ){
     temperature = map(ADCvoltage, 1550, 2000,460,630);
   } else if(ADCvoltage >= 885 && ADCvoltage < 1555){//1555 max voltage value (mV)
      temperature = map(ADCvoltage, 885, 1560,290,460); //ADCvoltage =  voltage value (mV), 885 = lower limit(mV), 1555 = higher limit(mv), 290 = lower limit(degree), 460 = higher limit(degree)  
   } else if(ADCvoltage >=  205&& ADCvoltage < 885) {
      temperature = map(ADCvoltage, 205, 885,100,289); 
   } else if (ADCvoltage >= 58 && ADCvoltage <205){
      temperature = map(ADCvoltage,58,205,50,99);
   } else if(ADCvoltage < 58) {
      temperature = map(ADCvoltage,0,58,25,49);  //25 considered as room temperature.
   } else {
      /* This will considerd as internal fault */
   } 
   
   LastReadTemp=temperature;
   return temperature;
}

/**********************************************************************************************************
                                uint16_t GetLastValue( void )        
**********************************************************************************************************
 Function:    uint16_t GetLastValue( void )     
 Input:       None
 Output:      uint16_t Temp in °C
 Discription: Returns the last read the Temp in °C
**********************************************************************************************************/
uint16_t TEMP_150500::GetLastValue( void ){
    return LastReadTemp;
}
