#include<SoftwareSerial.h>
#include <dht.h>
dht DHT;
const byte Rx_Pin = 10;
const byte Tx_Pin = 11;

const char CoAP_server[] = "117.60.157.137";     //CoAP server common IP addrss 
const int Port = 5683;                           //the server common IP addrss Port

static char retmcu[256] = {'\0'};               

#define Baud 9600
#define DHT11_PIN A0                           //define the DHT11
float temperature,humidity; //define variable for dpt11
int tem,hum;
SoftwareSerial Bc95_serial(Rx_Pin, Tx_Pin); //define software serial Pin

void setup() {
  BC95_init();
}

void loop() {
  read_dht();
  bc95_up_down_data();
  delay(5000);
}

void BC95_init()
{
  Serial.begin(Baud);
  Bc95_serial.begin(Baud);
  AT_cmd();
  AT_CFUN_close();
  delay(2000);
  AT_NCDP_setIP();
  delay(2000);
  AT_CFUN_open();
  delay(1000);
  AT_NRB_cmd();
  delay(5000);
  
  AT_CSQ_cmd();
  AT_CIMI_cmd();
  AT_CoAP_dealwith();
  }
void read_dht()
{
    char i = 15;                            //delay factor
    temperature = DHT.read11(DHT11_PIN);    //Read Tmperature data
    tem = DHT.temperature*1.0;             
    humidity = DHT.read11(DHT11_PIN);      //Read humidity data
    hum = DHT.humidity* 1.0;  
    while(i--)
    delay(1000);                            //delay 10 second
 }
void bc95_up_down_data()
{
  char cmd[32] = {'\0'};
  char cmd1[32] = {'\0'};
  Bc95_serial_clearbuf();
  Bc95_serial_clearbuf();
  snprintf(cmd, sizeof(cmd), "AT+NMGS=1,%d",tem);
  Bc95_serial.println(cmd); 
  bc95_serial_read1();
  Serial.println("the temperature is:");
  Serial.println(tem);
  }
 /**** read the cpu receive data buffer *****/
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
    //Serial.println(retmcu);  
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
    Serial.println(retmcu);  
}
 /**** clear the serial buffer *****/
void Bc95_serial_clearbuf()
{
    Bc95_serial.flush();
    while(Bc95_serial.available()) {
        Bc95_serial.read();
    }
}

void AT_cmd()
{
   Bc95_serial_clearbuf();
   Serial.println(F("send AT command."));
   Bc95_serial.println(F("AT"));
   bc95_serial_read();
  }
  
void AT_CFUN_close()
{
  Bc95_serial_clearbuf();
  //Serial.println(F("AT+CFUN=0"));
  Serial.println(F("close radio antena function"));
  Bc95_serial.println(F("AT+CFUN=0")); 
  bc95_serial_read();
  delay(1000);
  while(1)
  {
    Bc95_serial_clearbuf();
    Serial.println(F("AT+CFUN?"));
    Bc95_serial.println(F("AT+CFUN?"));
    bc95_serial_read();
    if(strstr(retmcu,"+CFUN:0") != NULL){
      Serial.println("close the sim is OK!");
      break;
      }
     else if(strstr(retmcu,"+CFUN:1") != NULL)
     {
      Serial.println("close the sim is NO OK!");
      continue;
      }else{}
    }
  }

void AT_NCDP_setIP()
{
  char cmd[32] = {'\0'};
  while(1){
    Bc95_serial_clearbuf();
    Serial.println(F("set CoAP common IP"));
    snprintf(cmd, sizeof(cmd), "AT+NCDP=%s,%d",CoAP_server, Port);
    Bc95_serial.println(cmd);
    bc95_serial_read();
    if(strstr(retmcu,"OK") != NULL){
      Serial.println(F("the Server IP was setup is OK"));
      break;
      }
  }
}
  
void AT_CFUN_open()
{
  Bc95_serial_clearbuf();
  Serial.println(F("AT+CFUN=1"));
  Bc95_serial.println(F("AT+CFUN=1")); 
  bc95_serial_read();
  delay(1000);
  while(1){
        Bc95_serial_clearbuf();
        Bc95_serial.println(F("AT+CFUN?"));
        bc95_serial_read();
        if(strstr(retmcu,"+CFUN:0") != NULL){
          Serial.println("open the sim is NO OK!");
          continue;
          }
        else if(strstr(retmcu,"+CFUN:1") != NULL)
        {
          Serial.println("open the sim is OK!");
          break;
          }
          else{}
    }
  }
void AT_NRB_cmd()
{
    Bc95_serial_clearbuf();
    Serial.println(F("sent the reset cmd"));
    Bc95_serial.println(F("AT+NRB"));
}
void AT_CSQ_cmd()
{
    while(1) {
        Bc95_serial_clearbuf();
        Serial.println(F("AT+CSQ"));
        Bc95_serial.println(F("AT+CSQ"));
        bc95_serial_read();
        if(strstr(retmcu, "+CSQ:99,99") != NULL) {    // res 99 is nosignal
            continue;
        } else if (strstr(retmcu, "+CSQ:") != NULL) {
            break;
        } else {
        }
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
 void AT_CoAP_dealwith()
 {
  while(1)
  {
    Bc95_serial_clearbuf();
    Serial.println(F("open the coap protol down data"));
    Bc95_serial.println(F("AT+NNMI=1"));
    if(strstr(retmcu, "OK") != NULL){
      Serial.println(F("the down data is display!"));
      break;
      }
    }
    while(1)
    {
      Bc95_serial_clearbuf();
      Serial.println(F("open the coap protol up data"));
      Bc95_serial.println(F("AT+NSMI=1"));
      if(strstr(retmcu, "OK") != NULL){
      Serial.println(F("the up data is display!"));
      break;}
      }
  }
  
