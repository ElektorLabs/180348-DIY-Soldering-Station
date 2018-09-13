/*********************************************************************************************************************
                   Project Name : SMD Soldering Station
**********************************************************************************************************************
MCU used     : ATmega32u4
Descriptoon  : A simple SMD soldering station is designed with an intension to make a compact version of a temperature
               control unit for the weller tip. This soldering device can be used to heat up the soldering bit to a 
               temperature ranging from 50 – 450 degree Celsius and also to check the temperature of the soldering bit.
***********************************************************************************************************************/

#include <EEPROM.h>

#include "HW_150500.h"
#include "enums.h"





/* The minimum input voltage we will run on */
/* This are common defines valid for all IRONs */
#define MAX_TEMP       ( 450 )
#define MIN_TEMP       ( 50 )

#define POWERSAVE_TIMEOUT ( 10 * 60 )

/* This is specific for 150500 */
#define VIN_MIN_MV     (11100)






 

/* everything that is volatile and need to be accessed accross functions */
volatile uint16_t timestamp=0;
volatile uint16_t setpoint =50;
volatile uint16_t powerSave_C =0;  
volatile fsmstate_t state=WELCOME_LOGO;
volatile uint8_t HeatPwr_Percent=0;
volatile uint16_t avg_power=0;
volatile uint16_t last_avg=0;
volatile uint8_t deltacount=0;
volatile uint8_t ErrNo=0;
volatile uint16_t LastActiveTemp=0;
volatile uint8_t RotaryEncoderLocked=0;
volatile uint16_t delay_ms = 0;
volatile uint16_t btn_press_time=0;

int32_t previous_PWM,current_PWM;
uint16_t display_Temp =0;
uint16_t temperature_Refresh =0;                       
bool powerSave_F = false;
bool powerStandby_F = false;

/* function prototypes */
void display_Sleep( void );
void rotary_EncoderEnable(void);
void rotary_EncoderDisable( void );
uint16_t read_StoreTemperature(void);
void write_StoreTemperature(uint16_t tempValue);   
void pwm_Adjust(void);
void Timer_1ms_Callback( void );
void powerSave_TimerReset( void );
void faultMonitor( uint8_t heatpwr_percent, uint16_t temperature , uint16_t target);
void set_delay(uint16_t ms);


HW_150500 Station;

/**********************************************************************************************************
                                void setup()        
**********************************************************************************************************
 Function:    void setup()
 Input:       None
 Output:      None 
 Description: This function is for device initialisation.
**********************************************************************************************************/
void setup()
{  
   
   
    setpoint = read_StoreTemperature();                 //Read previous saved temperature from EEPROM
    rotary_EncoderEnable();                             //Pin Change interrupt setting
    powerSave_TimerReset();                             //Reset timoutcounter
    Station.PWM.On(0);                                          //PWM On ( Zero Power ) 
    Station.Setup(Timer_1ms_Callback);                       //Hardwaresetup for the system                                  */  
    state=WELCOME_LOGO;                                 // The next state the FSM in the main loop will enter 

}

/**********************************************************************************************************
                                void loop()        
**********************************************************************************************************
 Function:    void loop()
 Input:       None
 Output:      NOne 
 Description: This function call repeately. Houses the FSM and the statelogic for standby and faults 
**********************************************************************************************************/
void loop() 
{
      
       switch(state){
         /*************************************************************
                               STATE WELCOME_LOGO       
          *************************************************************/
         case WELCOME_LOGO:{
           Station.Frontend.display_welcome_logo();
           set_delay(2000);                        /* delay for the next state */
           state = WELCOME_LOGO_WAIT; 
        
        } break;

        /*************************************************************
                              STATE WELCOME_LOGO_WAIT       
        *************************************************************/
        case WELCOME_LOGO_WAIT:{
           if(0==delay_ms){      
            state = WELCOME_TITLE;                  /* the next state the fsm will enter */
          } else if( digitalRead( ROTARY_BTN ) == LOW ){         /* if the user pressed the button we skip the intro */
            state= NOFAULT;
          }
        } break;

        /*************************************************************
                              STATE WELCOME_TITLE       
        *************************************************************/
        case WELCOME_TITLE:{
           Station.Frontend.display_title();
           set_delay(2000);                        /* delay for the next state */
           state = WELCOME_TITLE_WAIT; 
        } break;

        /*************************************************************
                              STATE WELCOME_TITLE_WAIT       
        *************************************************************/
        case WELCOME_TITLE_WAIT:{
          if( (0==delay_ms) || ( digitalRead( ROTARY_BTN ) == LOW ) ){ /* if the user pressed the button we skip the intro */
            state=NOFAULT;
          }
        } break;

        /*************************************************************
                              STATE NOFAULT       
        *************************************************************/
        case NOFAULT: {
          Station.Frontend.display_show_Temperatur(display_Temp,HeatPwr_Percent,setpoint,state,timestamp);
          write_StoreTemperature(setpoint);        //store "set temperature" in eeprom
          pwm_Adjust();
          Station.CheckLimits();   
          /* avg_pwr for powersave */
          avg_power= avg_power*15+HeatPwr_Percent;
          avg_power = avg_power/16;
          /* due to rounding errors this is not accurate but serves the purpose */
          if( avg_power != (last_avg) ) { /* We are awake */
            if(deltacount<255){
              deltacount++;
            } 
          } else {
            deltacount=0;
          }

           if( (deltacount>7) || (100 == HeatPwr_Percent ) || ( ( btn_press_time>100 ) && ( btn_press_time < 2000 ) ) ) { /* if we are used we prevent sleeping */
              powerSave_TimerReset();
           } else if( powerSave_F == true){ /* we need to go in powersave */
                powerSave_TimerReset();
                state = POWERSAVE;
           } 

          if(btn_press_time>5000){
                state = SLEEP;
                rotary_EncoderDisable();              /* we disable rotary encoder  */
                LastActiveTemp=setpoint;
               
          }
           
          last_avg=avg_power;
         
        } break;

       /*************************************************************
                              STATE TEMPSENS_FAIL       
        *************************************************************/   
        case TEMPSENS_FAIL:{   /* this is given by the fault monitor    */
          Station.PWM.Off();   /* we turn off the pwm                   */
          Station.Frontend.display_show_TempError(ErrNo); 
          state = WAIT;
        } break;

        /*************************************************************
                              STATE WAIT       
        *************************************************************/   
        case WAIT:{
            if(timestamp&0x0001){ /* we invert the display once a second */
              Station.Frontend.display_invert(1);//display.invertDisplay(1);
            } else {
              Station.Frontend.display_invert(0);//display.invertDisplay(0);
            }
           if( digitalRead( ROTARY_BTN ) == LOW )  /* if the user pressed the button we try to recover */
           {
            Station.Frontend.display_invert(0); //display.invertDisplay(0);
            state= RECOVER;
           }
           
        } break;


        /*************************************************************
                              STATE RECOVER       
        *************************************************************/   
        case RECOVER:{
          /* Recovery time my be displayed
           *  
           */
           Station.Frontend.display_show_Temperatur(display_Temp,HeatPwr_Percent,setpoint,state,timestamp);    //display temperature on oled, let the PWR string blink
           
        } break;

 
        /*************************************************************
                              STATE POWERSAVE       
        *************************************************************/           
        case POWERSAVE:{                        /* powersave state, will be entered after 15 minutes */
          rotary_EncoderDisable();              /* we disable rotary encoder  */
          Station.Frontend.display_show_Temperatur(display_Temp,HeatPwr_Percent,setpoint,state,timestamp);       
          LastActiveTemp=setpoint;                 /* we save the current setpoint */
          if(setpoint>100){                        /* we switch to 100 degree if the temp is currently higher */
            setpoint = 100;
          }
          state= POWERSAVE_WAIT;
        } break;

  
        /*************************************************************
                              STATE POWERSAVE_WAIT       
        *************************************************************/           
        case POWERSAVE_WAIT:{
          /* If we stay for more than 5 minutes here we will completly power down */
          Station.Frontend.display_show_Temperatur(display_Temp,HeatPwr_Percent,setpoint,state,timestamp);          // power save mode after 15 min
          pwm_Adjust();
          Station.CheckLimits();
          if( ( digitalRead( ROTARY_BTN ) == LOW ) && ( btn_press_time < 1000 ))
          {
            setpoint = LastActiveTemp;   /* we restore the temperatur */
            rotary_EncoderEnable();  /* we setup the encoder again */
            state= NOFAULT;           /* we switch to opperating state */
          } else if( true == powerSave_F ){   
              powerSave_TimerReset();    /* we reset the timer */
              state = SLEEP;          /* and we go to sleep */
          }
           
        } break;

       /*************************************************************
                              STATE SLEEP       
        *************************************************************/           
        case SLEEP:{
          Station.PWM.Off(); //Shutown the PWM 
          Station.Frontend.display_dim(true);//display.dim(true); //Dim the display, if supported 
          display_Sleep();   //Show the sleep screen
          setpoint=0;           //We set the target to 0 degree, seem safe
          HeatPwr_Percent=0;
          state = SLEEP_WAIT; 
          
        } break;

 
     /*************************************************************
                              STATE SLEEP_WAIT       
     *************************************************************/                  
        case SLEEP_WAIT:{
          display_Sleep();
          if( ( digitalRead( ROTARY_BTN ) == LOW ) && ( btn_press_time < 1000 ) ) 
           {
              state= NOFAULT;
              setpoint = LastActiveTemp;
             
              powerSave_TimerReset();    /* we reset the timer */
              rotary_EncoderEnable();
             Station.Frontend.display_dim(false);//display.dim(false);
           }
        } break;

    /*************************************************************
                              STATE default       
     *************************************************************/                      
        default:{
          
        } break;
      }
}

/*************************************************************************************************************
 *                                          set_delay()
 *************************************************************************************************************
 Function:    set_delay()
 Input:       None 
 Output:      None
 Description: Set the delaytimer to a given value 
 *************************************************************************************************************/    
void set_delay( uint16_t ms){
  while(delay_ms!=ms){
    delay_ms=ms;
  }
 
}

/*************************************************************************************************************
 *                                          rotary_EncoderSetting()
 *************************************************************************************************************
 Function:    rotary_EncoderSetting()
 Input:       None 
 Output:      None
 Description: Configure pin change interrupts for rotary encoder pin 
 *************************************************************************************************************/    
void rotary_EncoderEnable()
{   
    RotaryEncoderLocked=0;
}

/*************************************************************************************************************
 *                                          rotary_EncoderSetting()
 *************************************************************************************************************
 Function:    rotary_EncoderDisable()
 Input:       None 
 Output:      None
 Description: Disable pin change interrupts for rotary encoder pin 
 *************************************************************************************************************/    
void rotary_EncoderDisable()
{   
    RotaryEncoderLocked=1;
}



/**********************************************************************************************************
                                void read_Current()        
**********************************************************************************************************
 Function:    void read_Current()
 Input:       None
 Output:      None
 Discription: convert discrete ADC values to voltage in mV & PWM duty cycle calculation.
              if current exceeds 1.5A PWM is divided by 2.
**********************************************************************************************************/
 void read_Current()
 {
  
 
}  
 

/**********************************************************************************************************
                                void pwm_Adjust(void)        
**********************************************************************************************************
 Function:    void pwm_Adjust(void)
 Input:       None
 Output:      uint16_t 
 Description: Pwm adjustment
**********************************************************************************************************/
void pwm_Adjust(void)
{
  
    int16_t temp_Diff=0;
    uint16_t temperature=999;
    
    Station.PWM.Off();                          //switch off heater         
    _delay_ms(10);                              //wait for some time (to get low pass filter in steady state)
    temperature = Station.Temp.Read(ADC_AVG);
    
    if(setpoint > temperature)
    {
      temp_Diff = (int32_t)setpoint - (int32_t)temperature;
           
      if(temp_Diff < 3)
      {  

        current_PWM +=(int32_t)5;
        
      }
      else
      {
         
        current_PWM = (int32_t)temp_Diff  * (int32_t)setpoint/ (int32_t)6 ;
     
      }
      if(current_PWM > MAX_PWM_LIMIT)   
         current_PWM = MAX_PWM_LIMIT;
      previous_PWM = current_PWM;
        
    } 
    else if(setpoint < temperature)
    {
        temp_Diff = (int32_t)temperature - (int32_t)setpoint;
        
        if(temp_Diff > 2)
        {
         
          current_PWM = 0;

        }
        else
        {
          
          current_PWM -= (int32_t)current_PWM/(int32_t)7;
          previous_PWM = current_PWM;
        }
        
        if(current_PWM < 0)   
           current_PWM = 0;
        
    } else {
      current_PWM = previous_PWM;  
    }
    

    Station.PWM.On(current_PWM);
    HeatPwr_Percent= ((current_PWM*100) / MAX_PWM_LIMIT );
}

/*************************************************************************************************************
 *                                          powerSave_TimerReset()
 *************************************************************************************************************
 Function:    powerSave_TimerReset
 Input:       None 
 Output:      None
 Description: Reset the timer for powersave
 *************************************************************************************************************/ 
void powerSave_TimerReset(){

  while( powerSave_C != 0 ){
    powerSave_C =0;
  }
  powerSave_F=false;      /* we reset the powersave request */
}


/*************************************************************************************************************
 *                                          display_Sleep()
 *************************************************************************************************************
 Function:    display_Sleep()
 Input:       None 
 Output:      None
 Description: This will show the screensaver
 *************************************************************************************************************/    
void display_Sleep()
{
      Station.PWM.Off();     //switch PWM off until rotary switch is pressed
      /* Update display */
      Station.Frontend.display_show_sleep(timestamp);
      /* End of update */
      Station.Temp.Read(ADC_AVG); /* we now can safe read the temperatur */
      /* We refesh the temperatur for fault detection */
} 

/**********************************************************************************************************
                                Timer1_Callback        
**********************************************************************************************************
 Function:    Timer_1ms_Callback  
 Input:       None
 Output:      None 
 Description: Handels the fault monitor, powersave and the rotary encoder, updates the display temp
**********************************************************************************************************/
void Timer_1ms_Callback( void )
{
   static uint16_t onesecond_prescaler=0;
   static uint8_t input_a_buffer=0;
   static uint16_t calldelta=0; 

   if(delay_ms>0){
    delay_ms--;
   }

  if(  digitalRead( ROTARY_BTN ) == LOW ){
    if(btn_press_time<UINT16_MAX){
      btn_press_time++;
    }
  } else {
    btn_press_time=0;
  }

   /* We limit the buffer to 4bit / 4 sampels */ 
   input_a_buffer=input_a_buffer<<1;
   input_a_buffer=input_a_buffer&0x0F;
   
   if(LOW==digitalRead( ROTARY_TERM_B )){  
    input_a_buffer=input_a_buffer|0x00;
   } else {
    input_a_buffer=input_a_buffer|0x01;
   }

   if( (0==RotaryEncoderLocked) && (0x03==input_a_buffer) ){ /* Rising edge */
  /*  We ssume terminal B as stable
   *  if it is LOW now we moved CW else
   *  CCW
   */
    if( ROTARY_CW_LEVEL == digitalRead( ROTARY_TERM_A )){
       if(calldelta < 10) {
         /* We do nothing and ignore it, may bounced */
       } else if(calldelta<50){
        if(setpoint<(MAX_TEMP-5)){
          setpoint+=5;    
        } else {
          setpoint=MAX_TEMP;
        }     
      } else if ( calldelta < 250 ){
         if(setpoint<(MAX_TEMP-2)){
          setpoint+=2;    
        } else {
          setpoint=MAX_TEMP;
        }
      } else {
        if(setpoint<MAX_TEMP){
          setpoint++;    
        } 
      }
      calldelta=0;
         
    
    } else {
       if(calldelta < 10) {
         /* We do nothing and ignore it, may bounced */
       } else if(calldelta<50){
        if(setpoint>( MIN_TEMP + 5 ) ){
          setpoint-=5;    
        } else {
          setpoint=50;
        }     
      } else if ( calldelta < 250 ){
         if(setpoint>( MIN_TEMP + 2 ) ){
          setpoint-=2;    
        } else {
          setpoint=50;
        }
      } else {
        if(setpoint> MIN_TEMP){
          setpoint--;    
        } 
      }
      calldelta=0;
    }
    
  }

  /* This is to prevent an interger overflow */
   if(calldelta<UINT16_MAX){
    calldelta++;
   }

   /* sainity check */
   if(setpoint<MIN_TEMP){
      setpoint=MIN_TEMP;
   } else if( setpoint>MAX_TEMP){
      setpoint=MAX_TEMP;
   }
   
   if(onesecond_prescaler>1000){
       onesecond_prescaler=0;
       timestamp++;
       display_Temp = Station.Temp.GetLastValue();       
       if(powerSave_C == POWERSAVE_TIMEOUT )
       {
            powerSave_F = true;   
       }else if ( powerSave_C > POWERSAVE_TIMEOUT ) {
          _NOP();
       } else {
          powerSave_C++;
       }
       faultMonitor( HeatPwr_Percent, Station.Temp.GetLastValue(),setpoint);
   } else {
    onesecond_prescaler++;
   }

   
}
/*************************************************************************************************************
 *                                          read_StoreTemperature()
 *************************************************************************************************************
 Function:    read_StoreTemperature()
 Input:       None
 Output:      uint16_t 
 Description: Read previous saved temperature from EEPROM,
              If no previous value saved then set temperature to 50 degrees
 *************************************************************************************************************/    
uint16_t read_StoreTemperature()
{
   uint16_t saved_Temperature_1=0;

   saved_Temperature_1 = EEPROM.read(2);
   saved_Temperature_1 = saved_Temperature_1 << 8;
   saved_Temperature_1 = saved_Temperature_1 | EEPROM.read(3);
  
   if( ( saved_Temperature_1 > MAX_TEMP) || ( saved_Temperature_1 < MIN_TEMP) ){
    saved_Temperature_1= MIN_TEMP ;
   }
   return saved_Temperature_1;
   
}

/*************************************************************************************************************
 *                                          write_StoreTemperature()
 *************************************************************************************************************
 Function:    write_StoreTemperature()
 Input:       Nones
 Output:      uint16_t 
 Description: Read previous saved temperature from EEPROM,
              If no previous value saved then set temperature to 50 degrees
 *************************************************************************************************************/    
void write_StoreTemperature(uint16_t tempValue)
{
            EEPROM.update(2, ( ( tempValue & 0xFF00 ) >> 8 ) );
            EEPROM.update(3, ( ( tempValue & 0x00FF ) >> 0 ) );
}

/*************************************************************************************************************
 *                                          faultMonitor()
 *************************************************************************************************************
 Function:    faultMonitor
 Input:       
 Output:       
 Description: 
 *************************************************************************************************************/    
void faultMonitor( uint8_t heatpwr_percent, uint16_t temperature , uint16_t target){

  static uint16_t prev_temp=0;
  static uint8_t failcount=0; 
  static uint8_t recovery_delay=0;

  /* this is what we need only during the call of this function */
  int32_t delta_t_target= (int32_t)target-(int32_t)temperature;
  int32_t delta_t_temp=(int32_t)temperature-(int32_t)prev_temp;

  switch( state ) {
 
    case POWERSAVE:
    case POWERSAVE_WAIT:
    case NOFAULT:{
      /* we need the current temp, the dest temp and the heatpower */
      if( (heatpwr_percent>0) && (delta_t_target>5) ){
        /* We assume we are heating, as we havent reached the target now */
        if( (delta_t_temp<3) && ( delta_t_temp >=0 ) ){
          /* We take this as error */
          failcount++;
        } else {
          
          if(delta_t_temp> -10){
            failcount=0;
          } else {
             failcount++;
          }
        }  
      } else {
        /* seems working fine */
        failcount=0;
      }
      
      if(failcount>6){
         ErrNo=1;
         state=TEMPSENS_FAIL;
         recovery_delay=10;
       }

       if(temperature>600){
        ErrNo=3;
        state=TEMPSENS_FAIL;
        recovery_delay=10;
      }
    } 
    break;
    case SLEEP:
    case SLEEP_WAIT:{
  

      if(temperature>600){
        ErrNo=3;
        state=TEMPSENS_FAIL;
        recovery_delay=10;
      }
      
    } break;

    case TEMPSENS_FAIL:{
         if(recovery_delay>0){
              recovery_delay--;
         }
    } break;

    case WAIT:{
      if(recovery_delay>0){
              recovery_delay--;
         }
    } break;


    case RECOVER:{
      if(recovery_delay>0){
        recovery_delay--;
      } else {
        failcount=0;
        state=NOFAULT;
      }
      
    }

    default:{
      _NOP();
    } break;
  }
  
  prev_temp = temperature; 
}



