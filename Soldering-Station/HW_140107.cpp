#include "HW_140107.h"


void HW_140107::Setup( void* cb_1ms_Timer ){
    Frontend.display_setup();
    pinMode( ROTARY_BTN ,INPUT_PULLUP);              // Set rotary button as input and enable pullup
    /* this needs to be adjusted !*/
    Timer1.initialize(1000);                         //1ms interrupts from the timer
    Timer1.attachInterrupt(cb_1ms_Timer);            //Attatch the function to the isr callback
}

fsmstate_t HW_140107::CheckLimits(){
  fsmstate_t RetVal=NOFAULT;
  return RetVal;
}

