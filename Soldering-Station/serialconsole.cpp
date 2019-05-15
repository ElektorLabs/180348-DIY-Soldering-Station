#include <Arduino.h>
#include <avr/pgmspace.h>
#include "serailconsole.h"

uint8_t msgbuffer[64]={0,};
uint8_t msgbufferidx=0;
bool first_con = false;

/* A command can have three parts at the moment 
- set / get / help / clear  
- setpoint / temperatur / error 
- integer value ( 0 - 999 )
*/

typedef enum {
cmd_data_error=0,
cmd_data_get,
cmd_data_set,
cmd_data_clear,
cmd_data_help
} cmd_accesstype_t;


typedef enum {
    cmd_none = 0,
    cmd_setpoint,
    cmd_temperatur,
    cmd_error
} cmd_command_t;

typedef union {
        uint8_t u8;
        int8_t i8;
        uint16_t u16;
        int16_t i16;
        uint32_t u32;
        int32_t i32;
        float fl;
    }cmd_parameter_t;

typedef struct{
    cmd_accesstype_t cmd_dir;
    cmd_command_t command;
    cmd_parameter_t parameter;
} serial_command_t;


/* This is a bit hacky and needs to be solved by functionpointer */
extern void command_if_update_setpoint( uint16_t newsetpoint );
extern uint16_t command_if_get_setpoint( void );
extern uint16_t command_if_get_temperature(void  );
extern uint8_t command_if_get_error( void );
extern void command_if_clear_error( void );


/**********************************************************************************************************
                                void PrintErrorLocation()        
**********************************************************************************************************
 Function:    void PrintErrorLocation()
 Input:       uint8_t startidx, uint8_t current_index 
 Output:      None 
 Description: Show the location of a parsing error
**********************************************************************************************************/
void PrintErrorLocation(uint8_t startidx, uint8_t current_index  ){
          Serial.print(F(" Syntax error near:\""));
                        for(uint8_t c = startidx;c<current_index;c++){
                            if( msgbuffer[c]=='\n' ) {
                                Serial.print(F("[NEW LINE]"));
                            } else if ( msgbuffer[c]=='\r'){
                            Serial.print(F("[RETURN]"));
                            } else {
                                Serial.write( msgbuffer[c]);
                            }
                        }
                    Serial.println("\"");
}


/**********************************************************************************************************
                                void SerialConsolePrintWelcome()        
**********************************************************************************************************
 Function:    void SerialConsolePrintWelcome()
 Input:       None
 Output:      None 
 Description: Show the welcome banner
**********************************************************************************************************/
void SerialConsolePrintWelcome( void ){
  if(Serial){
    Serial.println(F("Elektor Solderingstation FW 1.2"));
    Serial.println(F("  on Hardware 180348 / 150500  "));
    Serial.println(F("------------------------------------------------------------------"));
    Serial.print(F(">"));
  }
}


/**********************************************************************************************************
                                void SerialCommandShowHelp()        
**********************************************************************************************************
 Function:    void SerialCommandShowHelp()
 Input:       None
 Output:      None 
 Description: Show the help
**********************************************************************************************************/
void SerialCommandShowHelp( void ){
    Serial.println(F("Supported commands"));
    Serial.println(F("------------------------------------------------------------------"));
    Serial.println(F("set/get setpoint [xxx] -> This will set or get the setpoint"));
    Serial.println(F("get temperature        -> This will report the current temp."));
    Serial.println(F("clear error            -> If an error is shown this will clear it"));
    Serial.println(F("help                   -> This will show this help"));
    Serial.println(F("-------------------------------------------------------------------"));
}



/**********************************************************************************************************
                                void SerialConsoleParseInput()        
**********************************************************************************************************
 Function:    void SerialConsoleParseInput()
 Input:       None
 Output:      serial_command_t 
 Description: Command parser, returns a struct with the parsed result
**********************************************************************************************************/
serial_command_t SerialConsoleParseInput( void ){
    serial_command_t command;
    command.cmd_dir = cmd_data_error;
    command.command = cmd_none;
    command.parameter.u32=0;
    if(msgbufferidx>sizeof(msgbuffer)){
        Serial.println( F("Buffer overflow") );
        for(uint8_t i=0;i<sizeof(msgbuffer);i++){
           msgbuffer[i]=0;
        }
        msgbufferidx=0;
    } else {
        uint8_t startidx = 0;
        uint8_t endidx = 0;
        for(uint8_t i=0;i<msgbufferidx;i++){
            if( ( msgbuffer[i]==' ') || (msgbuffer[i]=='\n') || (msgbuffer[i]=='\r') ){
                /* The first token has ended, we accept only 'set', 'get' and 'help' */
                if(strncmp_P(&msgbuffer[startidx] ,PSTR("set"), i)==0){
                    command.cmd_dir = cmd_data_set;
                } else if(strncmp_P(&msgbuffer[startidx] ,PSTR("get"), i)==0){
                    command.cmd_dir = cmd_data_get;
                } else if(strncmp_P(&msgbuffer[startidx] ,PSTR("help"), i)==0){
                    command.cmd_dir = cmd_data_help;
                } else if (strncmp_P(&msgbuffer[startidx] ,PSTR("clear"), i)==0){
                     command.cmd_dir = cmd_data_clear;
                } else {
                    command.cmd_dir = cmd_data_error;
                    PrintErrorLocation(startidx,i);
              
                }
                endidx=i;
                break;

                
                
            } else {
              endidx=i;
            }
        } 

        startidx = endidx+1;
        if( (startidx<msgbufferidx) && (msgbuffer[endidx]!='\n' ) &&  (msgbuffer[endidx]!='\r') )  {
          
        } else {
            /* Parsing is done */
            return command;
        }

        for(uint8_t i=startidx;i<msgbufferidx;i++){
            if( ( msgbuffer[i]==' ') || (msgbuffer[i]=='\n') || (msgbuffer[i]=='\r') ){
                /* The second token has ended, we accept only 'setpoint' , 'temperature' and 'error' */
                uint8_t tokenlenght = i - startidx;
                if(strncmp_P(&msgbuffer[startidx] ,PSTR("setpoint"), (tokenlenght) )==0){
                    command.command=cmd_setpoint;
                } else if(strncmp_P(&msgbuffer[startidx] ,PSTR("temperature"), (tokenlenght) )==0){
                    command.command=cmd_temperatur;
                } else if(strncmp_P(&msgbuffer[startidx] ,PSTR("error"), (tokenlenght) )==0){
                    command.command=cmd_error;
                } else {
                    command.command = cmd_none;
                        PrintErrorLocation(startidx,i);
                }
                endidx=i;
                break;
                
            } else {
                endidx=i;
            }
        }

         startidx = endidx+1;
        if( (startidx<msgbufferidx) && (msgbuffer[endidx]!='\n' ) &&  (msgbuffer[endidx]!='\r') )  {
          
        } else {
            /* Parsing is done */
            return command;
        }
        /*  
            last is to parse the parameter here 
            we currently only accept integer 
        */
        uint32_t value = 0;
        for(uint8_t i=startidx;i<msgbufferidx;i++){
            
            if( (msgbuffer[i]>='0') && (msgbuffer[i]<='9') ){
                value = value * 10;
                value = value + ( msgbuffer[i] -'0');
            } else {
                if( ( msgbuffer[i]=='\n') || (msgbuffer[i]=='\r') ){
                    command.parameter.u32 = value;
                    return command;
                }
                /* Parsing error */
                command.cmd_dir = cmd_data_error;
                command.command = cmd_none;
                command.parameter.u32=0;
                Serial.print(" Syntax error near:\"");
                PrintErrorLocation(startidx,i);
            }

                
        }

        return command;

    }

}


/**********************************************************************************************************
                                void SerialConsoleProcess()        
**********************************************************************************************************
 Function:    void SerialConsoleProcess()
 Input:       None
 Output:      None 
 Description: Processes incomming serial data
**********************************************************************************************************/
void SerialConsoleProcess( void ){
    if(Serial){
        if(first_con == false){
            SerialConsolePrintWelcome();
            first_con=true;
        }
        /* Grab data from interface */
        while ( ( Serial ) && (Serial.available() > 0)) {
                // read the incoming byte:
                uint8_t data = Serial.read();
                if( (data!='\r' ) && (data!='\n')){
                    Serial.write(data);
                }
            
                if(msgbufferidx<sizeof(msgbuffer) ){
                    msgbuffer[msgbufferidx]=data;
                    msgbufferidx++;
                } else {
                    /* we have an overflow */
                    msgbufferidx=sizeof(msgbuffer);
                }

                if( (data=='\r' ) || (data=='\n')){   /* Time to parse data */
                    Serial.print(F("\n\r"));
                    if( msgbufferidx == 0){
                       /* No Data in the buffer */ 
                    } else {
                        /* we need to parse the buffer */
                        serial_command_t command = SerialConsoleParseInput();

                        switch( command.cmd_dir ){
                            case cmd_data_help:{
                                SerialCommandShowHelp();
                            } break;

                            case cmd_data_set:{

                                switch ( command.command ){
                                    
                                    case cmd_setpoint:{
                                     if(command.parameter.u32<=UINT16_MAX){
                                            command_if_update_setpoint(command.parameter.u32);
                                            Serial.print(F("Setpoint:"));
                                            Serial.print(command_if_get_setpoint());  
                                            Serial.print(F("\n\rOK\n\r")); 
                                      }
                                    } break;

                                    default:{
                                        Serial.println("Not supported");
                                    }
                                }
                       
                            }break;

                            case cmd_data_get:{

                                  switch ( command.command ){
                                    
                                    case cmd_setpoint:{
                                    Serial.print(F("Setpoint:"));
                                    Serial.print(command_if_get_setpoint());  
                                    Serial.print(F("\n\rOK\n\r")); 
                                    } break;

                                    case cmd_temperatur:{
                                    Serial.print(F("Temperatur:"));
                                    Serial.print(command_if_get_temperature());  
                                    Serial.print(F("\n\rOK\n\r")); 
                                    } break;

                                    case cmd_error:{
                                    Serial.print(F("Error:"));
                                    Serial.print(command_if_get_error());  
                                    Serial.print(F("\n\rOK\n\r")); 

                                    } break;

                                    default:{
                                        Serial.println("Not supported");
                                    }
                                }

                            } break;

                            case cmd_data_clear:{

                                switch ( command.command ){
                                    
                                    case cmd_error:{
                                        command_if_clear_error();
                                        Serial.println("OK");
                                    } break;

                                    default:{
                                        Serial.println("Not supported");
                                    }
                                }
                          
                            }break;

                            default:{

                            } break;
                        }

                        /* we clear the buffer */

                        for (uint8_t i=0;i<sizeof(msgbuffer);i++){
                            msgbuffer[i]=0;
                        }
                        msgbufferidx = 0;
                        Serial.print(F("\r\n>"));

                    }
                }

        }     
    } else {
        first_con=false;
    }


    


}