#include "HW_150500.h"

void HW_150500::Setup( void* cb_1ms_Timer ){
    Frontend.display_setup();
    pinMode( ROTARY_BTN ,INPUT_PULLUP);              // Set rotary button as input and enable pullup
    Timer1.initialize(1000);                         //1ms interrupts from the timer
    Timer1.attachInterrupt(cb_1ms_Timer);            //Attatch the function to the isr callback
}

void HW_150500::CheckLimits(){
  if( 1500 <= Current.Read( ADC_AVG , PWM.ReadPWMValue() ) ){                    //read current (max limit 1.5 A)
  /* We need to limit the PWM here */
    PWM.On(MAX_PWM_LIMIT);
  }   

}

