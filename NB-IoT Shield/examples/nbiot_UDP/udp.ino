#include <SoftwareSerial.h> 
#include <stdlib.h>
#include <stdio.h>
                  
#define BAUDRATE 9600      //
const int timer = 15000; 
const byte rxpin = 10;     //Software Serial port
const byte txpin = 11;
const int port = 5683;     //server port,you should change the Port

const char udpserver[] = "120.78.138.177";  // UDP server of dragino,you change the IP
static char retmcu[256] = {'\0'}; 
const char data[]="474554202f666c7569642f73656e736f722f3f69643d323031373030303030303030333026696e5f74656d703d302e3030266f75745f74656d703d302e3030";                           
SoftwareSerial bc95_serial(rxpin, txpin); 
void setup() {                             //set the data rate for the Serial port
  bc95_init();
}

void loop()
{
  UDP_deal_with();
  delay(5000);
  }
void UDP_deal_with(){
  char cmd[512] = {'\0'};
  bc95_serial_clearbuf();
  //snprintf(cmd, sizeof(cmd),"AT+NSOST=0,%s,%d,
  snprintf(cmd, sizeof(cmd),"AT+NSOST=0,%s,%d,63,%s",udpserver,port,data);
  Serial.println(cmd);
  bc95_serial_read();
  bc95_serial.println(cmd);
  }

  
/*Function:    bc95_init()
 * para   :    NONE
 * return :    NONE
 * breif  :    init the module
 * data   :
 */
void bc95_init()
{
   Serial.begin(BAUDRATE);                // Use Serial Port for debug             
   bc95_serial.begin(BAUDRATE);  
   delay(2000);                  
   mcu_at_ok();
   delay(2000);
   mcu_ate0_cmd();
   delay(3000);                        
   mcu_at_csq();
   delay(3000);
   mcu_at_cereg();
   delay(3000);
   mcu_at_cgpaddr();
   delay(3000);
   mcu_at_socket();                       //set up the socket
   delay(3000);
  }
  
/*Function:    bc95_serial_read()
 * para   :    NONE
 * return :    NONE
 * breif  :    read the return data
 * data   :
 */
void bc95_serial_read()
{
    int i = 0;

    memset(retmcu, 0, sizeof(retmcu));

    bc95_serial.flush(); //Waits for the transmission of outgoing serial data to complete.

    delay(1500); //Waits for respones from server

    while(bc95_serial.available() && i < sizeof(retmcu) - 1) {
        retmcu[i] = bc95_serial.read();
        i++;
    }

    retmcu[i] = '\0';

    Serial.println(retmcu);  //for debug
}

/*Function:    bc95_serial_clearbuf()
 * para   :    NONE
 * return :    NONE
 * breif  :    clean the buff
 * data   :
 */
void bc95_serial_clearbuf()
{
    bc95_serial.flush();

    while(bc95_serial.available()) {
        bc95_serial.read();
    }
}
/*Function:    mcu_at_ok()
 * para   :    NONE
 * return :    NONE
 * breif  :    the function sent AT command to NB-modlue,ensure the module is ok
 * data   :
 */
void mcu_at_ok()
{
    while(1) {
        Serial.println("send AT command,wait for return \"OK\":");
        bc95_serial_clearbuf();                  
        bc95_serial.println("AT");            //send AT command to mcu
        bc95_serial_read();
        if (strstr(retmcu, "OK") != NULL) {       // res OK 
            delay(1000);
            break;
        }
    }
}


/*Function:    mcu_ate1_cmd()
 * para   :    NONE
 * return :    NONE
 * breif  :    the function sent "ATE1",close return display,ensure the nb module disappear messy code
 * data   :
 */
void mcu_ate0_cmd()
{
  Serial.println("close return display");
  while(1){
    Serial.println("---------begining--------->");
    bc95_serial_clearbuf();                       //clean the buff
    bc95_serial.println("ATE0");
    bc95_serial_read();                           //read the return data
    if(strstr(retmcu, "OK") != NULL){
      delay(1000);
      Serial.println("<-----------end---------");
      Serial.println("***succeed close return display****");
      break;
      }
    else{}
    }
  }

/*Function:    mcu_at_csq()
 * para   :    NONE
 * return :    NONE
 * breif  :    detect the modlue has signal,and wait for has signal
 * data   :
 */  
void mcu_at_csq()
{
  Serial.println("delect the module has signal?");
  Serial.println("---------begining--------->");
  while(1){
        delay(2000);                      //delay 2 second
       bc95_serial_clearbuf();
        Serial.println("\"AT+CSQ\" command ");
        bc95_serial.println(F("AT+CSQ"));
        bc95_serial_read();
        if(strstr(retmcu, "+CSQ:99,99") != NULL) {    // res 99 is nosignal
            continue;
        } else if (strstr(retmcu, "+CSQ:") != NULL) {
            ("<---------end--------");
            Serial.println("the module already signal.");
            break;
        } else {
            // Do Nothing
        }
    }
  }
/*Function:    mcu_at_cereg()
 * para   :    NONE
 * return :    NONE
 * breif  :    wait for module register
 * data   :
 */
void mcu_at_cereg()
{ 
    Serial.println("delect the module has register?");
    Serial.println("---------begining--------->");
    while(1)
    {
        bc95_serial_clearbuf();
        Serial.println(F("AT+CEREG?"));
        bc95_serial.println(F("AT+CEREG?"));
        bc95_serial_read();
        if(strstr(retmcu, "+CEREG:0,1") != NULL) {       // res +CEREG:n,stat , stat=2 indicate not registered
          Serial.println("<---------end---------");
          Serial.println("the module have register");
            break;
        }
        if(strstr(retmcu, "+CEREG:5,1") != NULL) {     
            break;
        }
    }
}
/*Function:    mcu_at_cgpaddr()
 * para   :    NONE
 * return :    NONE
 * breif  :    scan temporary public IP
 * data   :
 */
 void mcu_at_cgpaddr(){
  Serial.println("scan tempoaray public IP");
  bc95_serial_clearbuf();
  //Serial.println("AT+CGPADDR");
  bc95_serial.println(F("AT+CGPADDR"));
  bc95_serial_read();
  delay(2000);
  }
 /*Function:    mcu_at_socket()
 * para   :    NONE
 * return :    NONE
 * breif  :    set up the UDP socket,only need set up one
 * data   :
 */
 void mcu_at_socket()
 {
  Serial.println("sent up the socket");
  bc95_serial_clearbuf();
  Serial.println("AT+NSOCR= DGRAM,17,7777,1");
  bc95_serial.println("AT+NSOCR= DGRAM,17,7777,1");
  bc95_serial_read();
  }
//void mcu_at_socket()
//{
//  Serial.println("sent up the socket");
//  while(1){
//    bc95_serial_clearbuf();
//    Serial.println("AT+NSOCR= DGRAM,17,7777,1");
//    bc95_serial.println("AT+NSOCR= DGRAM,17,7777,1");
//    bc95_serial_read();
//    if(strstr(retmcu, "OK") != NULL){
//      delay(500);
//      Serial.println("succeed set up the socket.");
//      break;
//    }
//    else {
//      while(1){
//        delay(1000);
//        bc95_serial_clearbuf();                //AT+NSOCL=0 close the socke
//        Serial.println("close the module socket");
//        bc95_serial.println("AT+NSOCL=0");
//        if(strstr(retmcu, "OK") != NULL){
//          break;}
//         else{break;}
//        }
//      //bc95_serial_clearbuf();                //AT+NSOCL=0 close the socke
//      }
//    }
//  }


