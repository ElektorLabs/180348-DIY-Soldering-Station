#ifndef HW_150500_H_
 #define HW_150500_H_
 

#include <TimerOne.h>
#include "enums.h"
 /* This is for 150500 */
#include "frontend_150500.h"
#include "PWM_150500.h"
#include "TEMP_150500.h"
#include "CURRENT_150500.h"
#include "VIN_150500.h"


#define ROTARY_TERM_A ( 9 )
#define ROTARY_TERM_B ( 6 )
#define ROTARY_BTN ( 8 )

/* ROTARY_CW_LEVEL can be HIGH or LOW */
#define ROTARY_CW_LEVEL ( HIGH ) 

#define MAX_PWM        512
#define MAX_PWM_LIMIT  250
#define SAFE_PWM_VALUE 250
#define CURRENT_NO_ADJ 

#define CURRENT_LIMIT ( 1500 )

#define ADC_AVG        8

/* This is specific for 150500 */
#define VIN_MIN_MV     (10800)

/**********************************************************************************************************
                               CLASS HW_150500 
**********************************************************************************************************
 Baseclass:   SolderingStation Hardware
 Function:    Base for the HW Platform
 Input:       None
 Output:      None
 Discription: Basic class for the Hardware
**********************************************************************************************************/
class HW_150500 {
    public:
        HW_150500(){};
        FE_150500 Frontend;
        PWM_150500 PWM;
        TEMP_150500 Temp;
       

        fsmstate_t CheckLimits( void );
        void Setup( void* cb_1ms_Timer );
        void AdjustCurrent( uint16_t PWM_Value);
        void ShowUndervoltage();
        
    private:
      CURRENT_150500 Current;
      overcurrent_t HasOvercurrent( void );
      VIN_150500 Vin;
};

#endif
