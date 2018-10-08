#include "HW_150500.h"

#define VOLTAVGSIZE ( 16 ) 
volatile uint16_t VoltAVG[VOLTAVGSIZE] = { 0xFF,};

void HW_150500::Setup( void* cb_1ms_Timer ){
    Frontend.display_setup();
    pinMode( ROTARY_BTN ,INPUT_PULLUP);              // Set rotary button as input and enable pullup
    Timer1.initialize(1000);                         //1ms interrupts from the timer
    Timer1.attachInterrupt(cb_1ms_Timer);            //Attatch the function to the isr callback
}

overcurrent_t HW_150500::HasOvercurrent(  ){
  overcurrent_t Result;
  Result.limit = CURRENT_LIMIT;
  Result.value =  Current.Read( 2 );
  
  if(  Result.value > CURRENT_LIMIT  ){                    //read current (max limit 1.5 A)
  /* We need to limit the PWM here */
    Result.overcurrent = true;
  }  else {
    Result.overcurrent = false;
  }
  return Result;
}

fsmstate_t HW_150500::CheckLimits(){
  uint32_t avg_volt=0;
  fsmstate_t RetVal=NOFAULT;
   
  //Serial.println(".");
  //Serial.println( Current.Read( ADC_AVG  ));
  for(uint8_t i= 1 ;i <  VOLTAVGSIZE ; i++ ){
    VoltAVG[i-1] = VoltAVG[i];
  }
  VoltAVG[( VOLTAVGSIZE - 1 )]= Vin.Read( 4 );

  for(uint8_t i=0;i<  VOLTAVGSIZE ;i++){
    avg_volt+= VoltAVG[i];
  }
  avg_volt = avg_volt / VOLTAVGSIZE;
  
  if( avg_volt < 10800 ){
        /* We have undervoltage and need to signale it to the user */

        RetVal=UNDERVOLTAGE;
  }
  return RetVal;

}

