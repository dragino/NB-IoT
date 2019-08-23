#ifndef _common_h
#define _common_h
#include "Arduino.h"

#define T                   true
#define F                   false
#define BC95_STATE_TEST     "AT\n"
#define BC95_NRB            "AT+NRB\n"
#define BC95_CSQ            "AT+CSQ\n"
#define BC95_TIME           "AT+CCLK?\n"
#define BC95_NBAND          "AT+NBAND?\n"
#define BC95_CGATT_ON       "AT+CGATT=1\n"

#define BC95_CSCON          "AT+CSCON?\n"
#define BC95_CSCON_ON       "AT+CSCON=1\n"

#define BC95_NPSMR          "AT+NPSMR?\n"
#define BC95_NPSMR_ON       "AT+NPSMR=1\n"

#define BC95_CEREG          "AT+CEREG?\n"
#define BC95_NSOCL          "AT+NSOCL=1\n"

#define BC95_CFUN_state     "AT+CFUN?\n"
#define BC95_CFUN_ON        "AT+CFUN=1\n"
#define BC95_CFUN_OFF       "AT+CFUN=0\n"

#define BC95_NSOCR          "AT+NSOCR=STREAM,6,56000,1\n" //Prepare for the connection
#define BC95_NSOCO          "AT+NSOCO=1,52.203.155.126,1883\n" //Connect to Server,6002 is port
#define mqtt_end            "AT+NSOSD=1,2,E000\n"
#define BC95_NSORF          "AT+NSORF=1,10\n" //Revice message

class DATA
{
  public:
    char con[10] = {}; 
    char DATA[300]={};
    const char clientID[40] = "ee2c1c60-c54e-11e9-b01f-db5cf74e7fb7";
    const char userName[40] = "bab64ea0-9d5d-11e9-9636-f9904f7b864b";
    const char passWord[40] = "d231e547628d014fb31d7af7b3ad100013eed7e3";  
    uint8_t* conDataProcessing();
    uint8_t* sendDataProcessing(float val,int channel); 
  private:
    
};

class NB:DATA
{
  public:
    uint16_t faultCount;
    bool conDevice();  
    bool conCayenne(); 
    void serialInit(long baud = 9600);
    void deviceInit();   
    void sendCayenne(float _val,int _channel);
    void Text();
    void closeMqtt();
  private:
    String dataTransmission(String AT);
};



#endif
