#ifdef _PLATFORM_HW_140107_ 
#ifndef PWM_140107_H_
 #define PWM_140107_H_

 
 
 #include "Arduino.h"
 #include "enums.h"
 #include "pwm_hal.h"

 #define PWM_PIN        PC6
 
 
/**********************************************************************************************************
                               CLASS PWM_150500 
**********************************************************************************************************
 Baseclass:   SolderingStation_PWM
 Function:    Derrived class for 150500 PWM
 Input:       None
 Output:      None
 Discription: Handels the PWM of 150500
**********************************************************************************************************/
 class PWM_140107 : public SolderingStation_PWM {
        public:
        PWM_140107( ){ };
        void On( uint16_t value );
        void Off( void );
        uint16_t ReadPWMValue( void );
        
    };

#endif
#endif