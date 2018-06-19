#include<SoftwareSerial.h>

const byte Rx_Pin = 10;
const byte Tx_Pin = 11;

const char UDP_server[]="120.78.138.177";          //UDP server IP
const int Port = 5683;                             //UDP Port                 
static char retmcu[256] = {'\0'};

#define Baud 9600

SoftwareSerial Bc95_serial(Rx_Pin, Tx_Pin);
void setup()
{
  Bc95_UDP_init();
  }

  
void loop()
{
 UDP_up_down_data();
 delay(5000); 
}




  
void bc95_serial_read()
{
    int i = 0;
    memset(retmcu, 0, sizeof(retmcu));
    Bc95_serial.flush(); 
    delay(1500); 
    while(Bc95_serial.available() && i < sizeof(retmcu) - 1) {
        retmcu[i] = Bc95_serial.read();
        i++;
    }
    retmcu[i] = '\0';
    Serial.println(retmcu);  //for debug
}
void bc95_serial_read1()
{
    int i = 0;
    memset(retmcu, 0, sizeof(retmcu));
    Bc95_serial.flush(); 
    delay(1500); 
    while(Bc95_serial.available() && i < sizeof(retmcu) - 1) {
        retmcu[i] = Bc95_serial.read();
        i++;
    }
    retmcu[i] = '\0';
    //Serial.println(retmcu);  //for debug
}
void UDP_up_down_data()
{
  //delay(200);
  while(1){
  Bc95_serial_clearbuf();
  Bc95_serial.println(F("AT+NSOST=0,120.78.138.177,5683,7,44726167696E6F"));
  bc95_serial_read();
  if((strstr(retmcu,"+NSONMI:0,7")!=NULL)||(strstr(retmcu,"0,7")!=NULL))
  {
    delay(200);
    Bc95_serial_clearbuf();
    Bc95_serial.println(F("AT+NSORF=0,7"));
    bc95_serial_read1();
    delay(200);
    if(strstr(retmcu,"120.78.138.177")!= NULL)
    {
      Serial.println(retmcu);
      }else{
        }
    break;
    }else{}
  }
 // delay(1000);
  }
void Bc95_serial_clearbuf()
{
    Bc95_serial.flush();
    while(Bc95_serial.available()) {
        Bc95_serial.read();
    }
}  
void Bc95_UDP_init()
{
  Serial.begin(Baud);
  Bc95_serial.begin(Baud);
  delay(1000);
  AT_cmd();
  //AT_NBAND_query();
  AT_NRB_cmd();
  delay(5000);
  AT_CSQ_cmd();
  AT_CIMI_cmd();
  setup_Socket();
  }
//send the AT command
void AT_cmd()
{
   Bc95_serial_clearbuf();
   Serial.println(F("Send AT command ,in order to ensure the module can used "));
   Bc95_serial.println(F("AT"));
   bc95_serial_read();
  }
void AT_NRB_cmd()
{
    Bc95_serial_clearbuf();
    Serial.println(F("sent the reset cmd"));
    Bc95_serial.println(F("AT+NRB"));
}
void AT_CSQ_cmd()
{
  Serial.println(F("query the bc95-b5 module single strengh."));
    while(1) {
        Bc95_serial_clearbuf();
        //Serial.println(F("AT+CSQ"));
        Bc95_serial.println(F("AT+CSQ"));
        bc95_serial_read();
        if(strstr(retmcu, "+CSQ:99,99") != NULL) {    // res 99 is nosignal
            continue;
        } else if (strstr(retmcu, "+CSQ:") != NULL) {
          Serial.println(F("the module has signal.and start sent the cimi command "));
            break;
        } else {
        }
    }
}
void AT_NBAND_query()
{
  Serial.println(F("query the BC95-b5 work band:"));
  while(1){
    Bc95_serial_clearbuf();
    Bc95_serial.println(F("AT+NBAND"));
    bc95_serial_read();
    if(strstr(retmcu, "+NBAND:5") != NULL){
      break;}
    else if(strstr(retmcu,"ERROR") != NULL){
      continue;
      }else{}
    }
  }
void AT_CIMI_cmd()
{
  while(1){
    Bc95_serial_clearbuf();
    Serial.println(F("AT+CIMI"));
    Bc95_serial.println(F("AT+CIMI"));
    bc95_serial_read();
    if(strstr(retmcu, "OK") != NULL){
      break;}
    else if(strstr(retmcu,"ERROR") != NULL){
      continue;
      }else{}
    }
  }
void setup_Socket()
{
  Serial.println(F("set UDP_Socket Port"));
  //char cmd[32] = {'\0'};
  while(1){
    Bc95_serial_clearbuf();
    //snprintf(cmd, sizeof(cmd), "AT+NCDP=%s,%d",CoAP_server, Port);
    Bc95_serial.println(F("AT+NSOCR=DGRAM,17,5680,1"));
    bc95_serial_read();
    if(strstr(retmcu,"OK") != NULL){
      Serial.println(F("the server Socket was setup is OK"));
      break;
      }
    }
  }
 
