#include "HW_150500.h"

#define VOLTAVGSIZE ( 16 ) 
volatile uint16_t VoltAVG[VOLTAVGSIZE] = { 0xFF,};

/**********************************************************************************************************
                                void Setup()        
**********************************************************************************************************
 Function:    void Setup()
 Input:       void* 
 Output:      None
 Discription: Dose the init for the HW of the Station also sets the callback for the 1ms timer
**********************************************************************************************************/
void HW_150500::Setup( void* cb_1ms_Timer ){
    Frontend.display_setup();
    pinMode( ROTARY_BTN ,INPUT_PULLUP);              // Set rotary button as input and enable pullup
    Timer1.initialize(1000);                         //1ms interrupts from the timer
    Timer1.attachInterrupt(cb_1ms_Timer);            //Attatch the function to the isr callback
}



/**********************************************************************************************************
                                overcurrent_t HasOvercurrent()        
**********************************************************************************************************
 Function:    overcurrent_t HasOvercurrent()
 Input:       None
 Output:      overcurrent_t
 Discription: Checks if overcurrent condition occured
**********************************************************************************************************/
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

/**********************************************************************************************************
                                fsmstate_t CheckLimits()        
**********************************************************************************************************
 Function:    fsmstate_t CheckLimits()    
 Input:       None
 Output:      fsmstate_t
 Discription: Returns 'fault' states
**********************************************************************************************************/
fsmstate_t HW_150500::CheckLimits(){
  uint32_t avg_volt=0;
  fsmstate_t RetVal=NOFAULT;
   
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

/**********************************************************************************************************
                                void AdjustCurrent()        
**********************************************************************************************************
 Function:    void AdjustCurrent()    
 Input:       None
 Output:      fsmstate_t
 Discription: Returns 'fault' states
**********************************************************************************************************/
void  HW_150500::AdjustCurrent(uint16_t PWM_Value){
  static uint8_t Limit=100;
  uint16_t PWM_Raw = PWM_Value;
  
   overcurrent_t Overcurrent;
    if(Limit>100){
      Limit=100;
    }
    PWM_Value=(int32_t)PWM_Value * (int32_t)Limit;
    PWM_Value=(int32_t)PWM_Value / 100;
    
    if(PWM_Value>=MAX_PWM_LIMIT){
      PWM_Value = MAX_PWM_LIMIT;
    }
   PWM.On(PWM_Value);


    
   Overcurrent = HasOvercurrent();
    if(Overcurrent.overcurrent != false ){
      if(Limit<=0){   
        Limit=1;
      } else {
        Limit--; 
      }
      PWM_Value=(int32_t)PWM_Value * (int32_t)Limit;
      PWM_Value=(int32_t)PWM_Value / 100;
      
    } else {
      if(Limit<100){
        Limit++;
        
      }
      
    }
    PWM.On(PWM_Value);
}


/**********************************************************************************************************
                                void ShowUndervoltage()        
**********************************************************************************************************
 Function:    void ShowUndervoltage()    
 Input:       None
 Output:      None
 Discription: Show the undervoltage screen 
**********************************************************************************************************/
void HW_150500::ShowUndervoltage(){
  Frontend.display_show_Undervoltage(Vin.Read(8)); 
}

