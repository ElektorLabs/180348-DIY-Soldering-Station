#include <Arduino.h>
#include <avr/pgmspace.h>
#include "serailconsole.h"

uint8_t msgbuffer[64]={0,};
uint8_t msgbufferidx=0;
bool first_con = false;

/* A command can have three parts at the moment 
- set / get 
- setpoint / temperatur / error 
- integer value ( 0 - 999 )
*/

typedef enum {
cmd_data_error=0,
cmd_data_get,
cmd_data_set,
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


void SerialConsolePrintWelcome( void ){
  if(Serial){
    Serial.println(F("Elektor Solderingstation FW 1.1"));
    Serial.println(F("on Hardware 180348 / 150500"));
    Serial.println(F("Settings"));
    Serial.print(F("Setpoint:"));
    Serial.print(22);
    Serial.println(F("°C"));
  }
}

serial_command_t SerialConsoleParseInput( void ){
    Serial.print("Bytes to read:");
    Serial.print(msgbufferidx);
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
                } else {
                    command.cmd_dir = cmd_data_error;
                    Serial.print("Unknown:");
                    for(uint8_t z=0;z<i;i++){
                        Serial.write(msgbuffer[z]);
                    }
                    Serial.println("");
                }
                break;

                
                
            } else {
              Serial.write(msgbuffer[i])  ;
              endidx=i;
            }
        } 

        startidx = endidx+2;
        if(startidx<msgbufferidx){
                  
            Serial.print("Continue at:");
            Serial.print(startidx);
        } else {
            /* Parsing is done */
            Serial.println("End after fist stage");
            return command;
        }

        for(uint8_t i=startidx;i<msgbufferidx;i++){
            if( ( msgbuffer[i]==' ') || (msgbuffer[i]=='\n') || (msgbuffer[i]=='\r') ){
                /* The second token has ended, we accept only 'setpoint' , 'temperature' and 'error' */
                uint8_t tokenlenght = i-startidx;
                Serial.print("Lenght Toke2:");
                Serial.print(tokenlenght);
                Serial.println("");
                if(strncmp_P(&msgbuffer[startidx] ,PSTR("setpoint"), (tokenlenght) )==0){
                    command.command=cmd_setpoint;
                } else if(strncmp_P(&msgbuffer[startidx] ,PSTR("temperature"), (tokenlenght) )==0){
                    command.command=cmd_temperatur;
                } else if(strncmp_P(&msgbuffer[startidx] ,PSTR("error"), (tokenlenght) )==0){
                    command.command=cmd_error;
                } else {
                    command.command = cmd_none;
                }
                break;
                
            } else {
                endidx=i;
                 Serial.write(msgbuffer[i])  ;
            }
        }

        startidx = endidx;
        if(startidx<msgbufferidx){
                    startidx++;
        } else {
            return command;
        }

        /*  
            last is to parse the parameter here 
            we currently only accept integer 
        */

        uint32_t value = 0;
        for(uint8_t i=startidx;i<msgbufferidx;i++){
            value = value * 10;
            if( (msgbuffer[i]>='0') && (msgbuffer[i]<='9') ){
                value = value + ( msgbuffer[i] -'0');
            } else {
                if(msgbuffer[i]=='\r'){
                    return command;
                }
                /* Parsing error */
                command.cmd_dir = cmd_data_error;
                command.command = cmd_none;
                command.parameter.u32=0;
            }

                
        }

        return command;

    }

}

void SerialConsoleProcess( void ){
    if(Serial){
        if(first_con == false){
            SerialConsolePrintWelcome();
            first_con=true;
        }
        /* Grab data from interface */
        if (Serial.available() > 0) {
                // read the incoming byte:
                uint8_t data = Serial.read();
                Serial.write(data);
            
                if(msgbufferidx<sizeof(msgbuffer) ){
                    msgbuffer[msgbufferidx]=data;
                    msgbufferidx++;
                } else {
                    /* we have an overflow */
                    msgbufferidx=sizeof(msgbuffer);
                }

                if(data=='\n'){   /* Time to parse data */
                    if( msgbufferidx == 0){
                       /* No Data in the buffer */ 
                    } else {
                        /* we need to parse the buffer */
                        Serial.println(F("Process command"));
                        serial_command_t command = SerialConsoleParseInput();

                        switch( command.cmd_dir ){
                            case cmd_data_help:{
                                Serial.println(F("Help not implimented"));
                            } break;

                            case cmd_data_set:{
                                Serial.println(F("Set not implimented"));
                            }break;

                            case cmd_data_get:{
                                Serial.println(F("Get not implimented"));
                            } break;

                            default:{

                            } break;
                        }

                        switch ( command.command ){
                            case cmd_setpoint:{
                                Serial.println(F("Setpoint not implimented"));
                            } break;

                            case cmd_temperatur:{
                                Serial.println(F("Temperatur not implimented"));
                            } break;

                            case cmd_error:{
                                Serial.println(F("Error not implimented"));
                            } break;

                            default:{

                            }
                        }
                        /* we clear the buffer */

                        for (uint8_t i=0;i<sizeof(msgbuffer);i++){
                            msgbuffer[i]=0;
                        }
                        msgbufferidx = 0;

                    }
                }

        }     
    } else {
        first_con=false;
    }


    


}