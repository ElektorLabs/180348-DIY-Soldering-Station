#ifndef HW_150500_H_
 #define HW_150500_H_

#include <TimerOne.h>
#include "enums.h"
 /* This is for 150500 */
#include "frontend_150500.h"
#include "PWM_150500.h"
#include "TEMP_150500.h"
#include "CURRENT_150500.h"


#define ROTARY_TERM_A ( 9 )
#define ROTARY_TERM_B ( 8 )
#define ROTARY_BTN ( 6 )

/* ROTARY_CW_LEVEL can be HIGH or LOW */
#define ROTARY_CW_LEVEL ( HIGH ) 

#define MAX_PWM        512
#define MAX_PWM_LIMIT  250

#define ADC_AVG        20

class HW_150500 {
    public:
        HW_150500(){};
        void CheckLimits( void );
        void Setup( void* cb_1ms_Timer );
        FE_150500 Frontend;
        PWM_150500 PWM;
        TEMP_150500 Temp;
    private:
      CURRENT_150500 Current;
};

#endif
