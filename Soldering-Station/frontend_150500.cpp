#include "frontend_150500.h"
#include "SSD1306.h"
#include "GFX.h"
#include "logo.h"
#include "xbm_img.h"
#include "animation.h"

/* Display for 150500 */
Adafruit_SSD1306   display(OLED_DATA, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/**********************************************************************************************************
                                void display_setup()        
**********************************************************************************************************
 Function:    void display_setup()
 Input:       None
 Output:      None
 Discription: Display setup
**********************************************************************************************************/
void FE_150500::display_setup( void ){

    /* This is for 150500 only */
    display.begin(SSD1306_SWITCHCAPVCC);
    display.clearDisplay();      
    display.display();
}

/**********************************************************************************************************
                                void display_welcome_logo()        
**********************************************************************************************************
 Function:    void display_welcome_logo()
 Input:       None
 Output:      None
 Discription: Shows the welcome logo
**********************************************************************************************************/
void FE_150500::display_welcome_logo( void ) {

    display.clearDisplay();
    display.drawBitmap(0,0,logo,128,64,WHITE);
    display.display();
     
}

/**********************************************************************************************************
                                void display_title()        
**********************************************************************************************************
 Function:    void display_title()
 Input:       None
 Output:      None
 Discription: Shows the welcome title
**********************************************************************************************************/
void FE_150500::display_title( void ){

    display.clearDisplay();
    display.drawBitmap(0,0,title,128,64,WHITE);
    display.display();       
   

}

/**********************************************************************************************************
                                void display_invert()        
**********************************************************************************************************
 Function:    void display_invert()
 Input:       None
 Output:      None
 Discription: inverts the display
**********************************************************************************************************/
void FE_150500::display_invert( bool invert ) {

    display.invertDisplay(invert);

}

/**********************************************************************************************************
                                void display_dim()        
**********************************************************************************************************
 Function:    void display_dim()
 Input:       bool dim
 Output:      None
 Discription: Dims the display if parmaeter is true, otherwyse restores brightness
**********************************************************************************************************/
void FE_150500::display_dim(bool dim ){

    display.dim(dim);
    
}

/**********************************************************************************************************
                                void display_show_Temperatur()        
**********************************************************************************************************
 Function:    void display_show_Temperatur()
 Input:       uint16_t dispTemperature, uint8_t HeatPwr_Percent, uint16_t setpoint, fsmstate_t state, uint16_t timestamp
 Output:      None
 Discription: Shows the Mainscreen with temperatur
**********************************************************************************************************/
void FE_150500::display_show_Temperatur(uint16_t dispTemperature, uint8_t HeatPwr_Percent, uint16_t setpoint, fsmstate_t state, uint16_t timestamp ){
    
     display.clearDisplay();

    display.setTextSize(1);  
    display.setCursor(0,0); 
    /* This is for some state specialized */                  
    if( (RECOVER==state) && ( timestamp & 0x0001) ){
      /* We won't display the pwr string */
    }else{
      display.print("PWR "); 
    }

    /* if we have entered powersave we will print it */
    if(state==POWERSAVE_WAIT){
          display.setCursor(12,16);                   
          display.print(" Powersave Mode");
    }
   
    display.drawRect(20,0,50,8,WHITE);
    display.fillRect(20,0,(HeatPwr_Percent/2),8,WHITE);
    display.setTextColor(WHITE);
 
    display.setTextColor(WHITE);
    display.setTextSize(2);  

    if( (state==POWERSAVE_WAIT)  && ( timestamp & 0x0001) ){
      
    } else {
      display.setCursor(90,0);                    //Display set temperature
      display.print(setpoint);
    }
    display.setCursor(85,22);
    display.print("o");
    
    display.setTextSize(4); 
    display.setCursor(0,32);
    display.print(dispTemperature);          //display curremt temperature of iron
    display.setCursor(100,32);
    display.print("C");

    //display.fillRect(0,62,(POWERSAVE_TIMEOUT-powerSave_C)*128/POWERSAVE_TIMEOUT,2,WHITE);
    
  
    display.display();
}

/**********************************************************************************************************
                                void display_show_TempError()        
**********************************************************************************************************
 Function:    void display_show_TempError()
 Input:       uint8_t ErrNo
 Output:      None
 Discription: Shows the temperaturerror screen
**********************************************************************************************************/
void FE_150500::display_show_TempError(uint8_t ErrNo){

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.drawXBitmap(0,0,TempFault_bits,TempFault_width,TempFault_height,WHITE);
      display.setTextSize(1); 
      display.setCursor(64,0);
      display.print("Soldertip");

      /* We now display the fault case */
      display.setCursor(64,12);
      display.print("FAULT:E");
      display.print(ErrNo);
      display.display();  

}

/**********************************************************************************************************
                                void display_show_TempError()        
**********************************************************************************************************
 Function:    void display_show_TempError()
 Input:       uint8_t ErrNo
 Output:      None
 Discription: Shows the temperaturerror screen
**********************************************************************************************************/
void FE_150500::display_show_Undervoltage(uint16_t Vin){

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1); 
      display.setCursor(16,0);
      display.print("Undervoltage");

      /* We now display the fault case */
      display.setCursor(16,12);
      display.print("Input: ");
      display.print(Vin/1000);
      display.print(".");
      display.print(Vin%1000);
      display.print("V");
      display.display();  

}

/**********************************************************************************************************
                                void display_show_sleep()        
**********************************************************************************************************
 Function:    void display_show_sleep()
 Input:       uint16_t timestamp
 Output:      None
 Discription: Displays the sleep animaton, depending on the timestamp
**********************************************************************************************************/
void FE_150500::display_show_sleep( uint16_t timestamp){

      static uint16_t last_timestamp=0;
      static uint8_t offset_x=0;
      static uint8_t dir=0;
      
    
      display.clearDisplay();
      /* every second move the image a bit arround */
      if(last_timestamp!=timestamp){
        last_timestamp=timestamp;  
        if(dir==0){
            if(offset_x<38){
              offset_x++;
            }else{
              dir=1;
            }         
        } else {
          if(offset_x>0){
            offset_x--;
          } else {
            dir=0;
          }
        }
      }
      /* depending on the timestamp show one of the four animation images */
      switch(timestamp&0x0003){
        case 0:{
          display.drawXBitmap(offset_x,0,animation_0,animation_width,animation_height,WHITE);
        } break;

        case 1:{
          display.drawXBitmap(offset_x,0,animation_1,animation_width,animation_height,WHITE);
        } break;

        case 2:{
          display.drawXBitmap(offset_x,0,animation_2,animation_width,animation_height,WHITE);
        } break;

        case 3:{
           display.drawXBitmap(offset_x,0,animation_3,animation_width,animation_height,WHITE);
        } break;

        default:{
          /* we display nothing */
        } break;
      }                  
      display.display();


    
}
