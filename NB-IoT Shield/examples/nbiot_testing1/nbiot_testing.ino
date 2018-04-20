
#include <SoftwareSerial.h>

const int timer = 15000;  //timer, default: six minites; 
/****define Serial port*****/
const byte rxpin = 10;    //Software Serial port
const byte txpin = 11;

const char server[] = "180.101.147.115";   //CDP server of China Telecom
const int port = 5683;                     //server port

const char udpserver[] = "120.78.138.177"; // UDP server of dragino

static char retmcu[256] = {'\0'};    //save the mcu return message for AT Command

static byte reboot = 0;

static char IMEI[16] = {'\0'}; 


#define MAXLEN 256      // MAX Payload length                  

#define CHARLEN 32                       

#define BAUDRATE 9600   //define the baud is 9600bps/s

void mcu_init();        //CPU初始化

int mcu_at_nmgs(int, char *);

void send_data_udp(int, char *);

void read_downstream();

char *mcu_get_imei(char *);

SoftwareSerial bc95_serial(rxpin, txpin);       // RX  TX
/**************************************************************************************
 * funciton name  ：setup()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：initialize Arduino's function of setu 
 * data           ：2018/4/16
*****************************************************************************************/
void setup() {                              //set the data rate for the Serial port  
    Serial.begin(BAUDRATE);                // Use Serial Port for debug(config the baud of uart is 9600bps/s)          
    bc95_serial.begin(BAUDRATE);            //config module of bc95 baud of uart is 9600bps/s too.                
    mcu_init();                             //module were initialized. 
    delay(1000);                            //Arduino delay 1 second     
}
/**************************************************************************************
 * funciton name  ：loop()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：NB-IoT init and send for data to platform
 * data           ：2018/4/16
*****************************************************************************************/
void loop() {
    char temp[] = "Send a UDP datagram containing length bytes of data";      //the array was defined ,the data was straged on it
    Serial.println(F("Send a message use UDP Protocol"));                    //computer will printed to char string 
    send_data_udp(strlen(temp), temp);                                        //send to udp server
    delay(1000);                                                              //delay 1 second  
    Serial.println(F("Send a message use CoAP Protocol"));                   //computer will printed to char string 
    mcu_at_nmgs(3, "020103");                                                 //send message to CoAP server, for testing
    delay(timer);                                                             //delay(1500)
    read_downstream();
    delay(1000);                               
}
/**************************************************************************************
 * funciton name  ：bc95_serial_read()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：data was straged in retmcu array
 * data           ：2018/4/16
*****************************************************************************************/
void bc95_serial_read()
{
    int i = 0;

    memset(retmcu, 0, sizeof(retmcu));//reference c language lib menset
                                      //array is cleaned in here
    bc95_serial.flush();              //Waits for the transmission of outgoing serial data to complete.

    delay(1500);                     //Waits for respones from server

    while(bc95_serial.available() && i < sizeof(retmcu) - 1) {
        retmcu[i] = bc95_serial.read();//read return's value and strage in this array
        i++;
    }

    retmcu[i] = '\0';                  //

    Serial.println(retmcu);      //for debug
}
/**************************************************************************************
 * funciton name  ：bc95_serial_clearbuf()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：clear the buffer
 * data           ：2018/4/16
*****************************************************************************************/
void bc95_serial_clearbuf()
{
    bc95_serial.flush();                     

    while(bc95_serial.available()) {
        bc95_serial.read();
    }
}
 /**************************************************************************************
 * funciton name  ：str2hex()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：conver strings to ascii code
 * data           ：2018/4/16
*****************************************************************************************/
void str2hex(char *hexstr, int hexlen, char *str, int strlen)
{
    int i;
    char tmp1[4] = {'\0'};
    for (i = 0; i < strlen - 1; i++) {
        sprintf(tmp1, "%x", str[i]);
        strncat(hexstr, tmp1, hexlen);
    }
}

 /**************************************************************************************
 * funciton name  ：mcu_at_nrb()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：Sent the AT command
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_nrb()
{
    bc95_serial_clearbuf();            //clear the buffer
    Serial.println(F("Reboot BC95")); //
    bc95_serial.println(F("AT+NRB"));  //send AT+NRB command to NB-IoT,this command is that module will be reboot
}
 /**************************************************************************************
 * funciton name  ：mcu_at_ok()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：Sent the AT command
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_ok()
{
    while(1) {
        bc95_serial_clearbuf();                  //clear the buffer             
        bc95_serial.println(F("AT"));            //send "AT"command to NB-IoT
        bc95_serial_read();                      //read the return value
        if (strstr(retmcu, "OK") != NULL) {      //if return value is "OK" than program delay 1second jump out loop
            delay(1000);
            break;
        }
    }
}

 /**************************************************************************************
 * funciton name  ：mcu_get_cimi
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：Sent the AT+CIMI command
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_get_cimi()
{
    while(1) {
        bc95_serial_clearbuf();                      //flush bc95_serial
        Serial.println(F("AT+CIMI"));               //
        bc95_serial.println(F("AT+CIMI"));           //send AT+CIMI command to NB-IoT module
        bc95_serial_read();                          //read the return value
        break;                                       //jump out of loop
    }
}
/**************************************************************************************
 * funciton name  ：mcu_get_imei
 * parameter      ：*char
 * return value   ：*char 
 * effect         ：gain the IMEI code
 * data           ：2018/4/16
*****************************************************************************************/
char *mcu_get_imei(char *imei)
{
    int i;
    char *pt;                                 //this pointer in order to recevied the IMEI code take in imei array

    bc95_serial_clearbuf();                   //clear the buffer                  
    Serial.println(F("AT+CGSN=1"));           //
    bc95_serial.println(F("AT+CGSN=1"));      //send the "AT+CGSN=1"command,then the return value is IMEI code
    bc95_serial_read();                       //read return code

	//judge if have return value of IMEI code
    if (strstr(retmcu, "+CGSN") != NULL) { /* respones format: +CGSN:86370303110???? */          
        pt = strrchr(retmcu, ':');            //    
        for(i = 0; i < sizeof(IMEI); i++) {
          imei[i] = pt[i+1];                  //store the IMEI code to imei array.
        }
        return imei;                          //return the imei array
    }

    return NULL;                             //or return NULL                     
}

/**************************************************************************************
 * funciton name  ：mcu_at_nband()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：Arduino sent"AT +NBAND"to NB-IoT,we choose 850Mhz working frequency 
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_nband()
{
    while(1) {                                            
        bc95_serial_clearbuf();                            //clean the buffer to NB-IoT 
        Serial.println(F("AT+NBAND?"));                   //Arduino print "AT+NBAND?" char string to computer
        bc95_serial.println(F("AT+NBAND?"));               //Arduino send "AT+NBAND?"command to NB-IoT,in order to looking the module work in state
        bc95_serial_read();                                //read the return value
        if(strstr(retmcu, "+NBAND:5") != NULL) {           //Read the retmcu array,Arduino start judge the value appear nbnber"5"
            break;                                         //if the value is number"5",NB-IoT working frequency is 850Mhz,jump out of the loop.
        } else {
            Serial.println(F("AT+NBAND=5"));               //if the return value isn't number"5",afterward,Arduino sent "AT+NBABD=5"command to NB-Iot module.            
            bc95_serial.println(F("AT+NBAND=5"));          // AT+NBABD=5 command is chose the mode that NB-IoT working frequency is 850Mhz         
            reboot = 1;                                    //reboot laber bit assignment is "1". 
            break;                                         //program jumps out of the loop
        }
    }
}
/**************************************************************************************
 * funciton name  ： mcu_at_cgdcont()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：Define a PDP Context * notice:
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_cgdcont()
{
    bc95_serial_clearbuf();                    
    Serial.println(F("AT+CGDCONT=1,IP,CTNB"));
    bc95_serial.println(F("AT+CGDCONT=1,\"IP\",\"CTNB\""));      
    delay(1000);
    bc95_serial_clearbuf();                    
    Serial.println(F("AT+CGDCONT?"));           
    bc95_serial.println(F("AT+CGDCONT?"));       
    bc95_serial_read();
}
/**************************************************************************************
 * funciton name  ：mcu_at_nconfig()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send AT+NCONFIG command,look for NB-IoT message of config
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_nconfig()
{
    while(1) {
        bc95_serial_clearbuf();                                    //clear the buffer
        Serial.println(F("AT+NCONFIG?"));                         //
        bc95_serial.println(F("AT+NCONFIG?"));		                 //send "AT+NCONFIG"command
        bc95_serial_read();                                        //return the value
        if(strstr(retmcu, "+NCONFIG:AUTOCONNECT,FALSE") != NULL) { //if return value isn't"+NCONFIG:AUTOCONNECT,FALSE",this isn't auto look for network......
            Serial.println(F("AT+NCONFIG=AUTOCONNECT,TRUE"));     //
            bc95_serial.println(F("AT+NCONFIG=AUTOCONNECT,TRUE")); //then send for"AT+NCONFIG=AUTOCONNECT,TRUE"command to NB-IoT
            reboot = 1;                                            //1reboot module laber is "1"
            break;                                                 //break the while(1)
        } else if (strstr(retmcu, "+NCONFIG:AUTOCONNECT,TRUE") != NULL) {//
            break;                                                       //
        } else {                                                         //this is else if wait for appear return is ture
            //Do Nothing
        }
    }
}
/**************************************************************************************
 * funciton name  ：mcu_at_csq()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send "AT+CSQ"command,look for singal strengh
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_csq()
{
    while(1) {
        bc95_serial_clearbuf();
        Serial.println(F("AT+CSQ"));
        bc95_serial.println(F("AT+CSQ"));
        bc95_serial_read();
        if(strstr(retmcu, "+CSQ:99,99") != NULL) {           // res 99 is nosignal
            continue;
        } else if (strstr(retmcu, "+CSQ:") != NULL) {
            break;
        } else {
            // Do Nothing
        }
    }
}
/**************************************************************************************
 * funciton name  ：mcu_at_cgatt()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send "AT+CGATT"command,detect network if activate
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_cgatt()
{
    while(1)
    {
        bc95_serial_clearbuf();                         //clear the send buffer
        Serial.println(F("AT+CGATT?"));
        bc95_serial.println(F("AT+CGATT?"));            //"AT+CGATT"command
        bc95_serial_read();                             //return the value
        if(strstr(retmcu, "+CGATT:1") != NULL) {        //return value 1：active，0：disavtive
            break;                                      //active break while
        }
    }
}            
/**************************************************************************************
 * funciton name  ：mcu_at_cfun()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send "AT+CFUN"command,enable Receive and send
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_cfun()
{
    while(1) {
        bc95_serial_clearbuf();                 //clear the buffer
        Serial.println(F("AT+CFUN?"));
        bc95_serial.println(F("AT+CFUN?"));     //send "AT+CFUN"command to NB-IoT module
        bc95_serial_read();                     //read return value
        if(strstr(retmcu, "+CFUN:1") != NULL) { //if return value is"1",reprense NB-IoT enable receive and send port,at the moment break the while
            break;
        } else {                                //or wait for return is"1"
            //Do Nothing
        }
    }
}
/**************************************************************************************
 * funciton name  ：mcu_at_cereg()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send "AT+CEREG"command,look for EPS network is register state
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_cereg()
{
    while(1)
    {
        bc95_serial_clearbuf();                     //clear the buffer
        Serial.println(F("AT+CEREG?"));
        bc95_serial.println(F("AT+CEREG?"));        //send "AT+CEREG?"command to NB-IoT
        bc95_serial_read();
        if(strstr(retmcu, "+CEREG:0,1") != NULL) {  // res +CEREG:n,stat , stat=2 indicate not registered
            break;
        }
        if(strstr(retmcu, "+CEREG:5,1") != NULL) {     
            break;
        }
    }
}
/**************************************************************************************
 * funciton name  ：mcu_at_cscon()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send "AT+CSCON"command
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_cscon()
{
    while(1)
    {
        bc95_serial_clearbuf();
        Serial.println(F("AT+CSCON?"));
        bc95_serial.println(F("AT+CSCON?"));
        bc95_serial_read();
        if(strstr(retmcu, "+CSCON:0,1") != NULL) {  //res +CSCON:n,mode , if mode=1 connected, 0 idle
            break;
        }
    }
}

/**************************************************************************************
 * funciton name  ：mcu_at_ncdp()
 * parameter      ：NONE
 * return value   ：NONE 
 * effect         ：send "AT+NCDP"command,config and look for CDP server
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_ncdp()
{
    while(1) {
        bc95_serial_clearbuf();
        Serial.println(F("AT+NCDP?"));
        bc95_serial.println(F("AT+NCDP?"));
        bc95_serial_read();                 //query the "AT+NCDP"command,read return value
        if(strstr(retmcu, server) != NULL)  //return value is not NULL,break the while
            break;
        else {
            bc95_serial_clearbuf();
            Serial.println(F("AT+CFUN=0"));
            bc95_serial.println(F("AT+CFUN=0"));
            bc95_serial_read();            //send "AT+CFUN=0"command and read return value
            if(strstr(retmcu, "OK") != NULL) {    // 
                char cmd[32] = {'\0'};
                snprintf(cmd, sizeof(cmd), "AT+NCDP=%s,%d",server, port);
                bc95_serial.println(cmd);
                bc95_serial_read();
                if(strstr(retmcu, "OK") != NULL) {    // 
                    bc95_serial.println(F("AT+CFUN=1"));
                    delay(500);
                    break;
                }
            }
        }
    }
}
/****************************************************************
 * desc: send a message from the terminal to network via connect 
 *  device platform server. (CDP)
 * ret: return 0  if send message success
*************************************************************** */
int mcu_at_nmgs(int len, char *data)
{
    char cmd[256] = {'\0'};
    snprintf(cmd, sizeof(cmd), "AT+NMGS=%d,%s", len, data);
    bc95_serial_clearbuf();
    Serial.println(cmd);
    bc95_serial.println(cmd);
    delay(1000);  //wait 1 sencond
    bc95_serial_read();
    if(strstr(retmcu, "OK") != NULL) {        
        return 0;
    }
    return 1;
}

/****************************************************************
 * desc: Queries the status of downstream messages received from 
 *       the CDP server
 *
 * Res: BUFFERED=<num>, RECENIVED=<num>, DROPPED=<num>
 *      +CME ERROR:<err>
 *  
*************************************************************** */
void mcu_at_nqmgr()
{
    bc95_serial_clearbuf();
    Serial.println(F("AT+NQMGR"));
    bc95_serial.println(F("AT+NQMGR"));
    bc95_serial_read();
}


/****************************************************************
 * desc: Queries the status of downstream messages received from 
 *       the CDP server
 * note: 
 *  
 *
*************************************************************** */
             
void read_downstream()
{
    int i; 
    char *sp, buf[8];

    mcu_at_nqmgr();

    if ((strstr(retmcu, "BUFFERED=")) != NULL) {

        sp = strchr(retmcu, '=');

        for (i = 0; sp[i] != ',', i < 8; i++) {
            buf[i] = sp[i + 1];
        }

        buf[i] = '\0';

        if (atoi(buf) > 0) 
            mcu_at_nmgr();
    }
}
/****************************************************************
 * desc: Queries the status of the upstream messages 
 *       sent to the CDP server
 * note: 
 * Res: PENDING=<num>, SENT=<num>, ERROR=<num> 
 *
*************************************************************** */
void mcu_at_nqmgs()
{
    bc95_serial_clearbuf();
    Serial.println(F("AT+NQMGS"));
    bc95_serial.println(F("AT+NQMGS"));
    bc95_serial_read();
}


/****************************************************************
 * desc: return the oldest buffered message and deletes from buffer 
 * note: if new message indications(AT+NNMI)are turn on then received
 *  messages will not be available via this command
 *
*************************************************************** */
void mcu_at_nmgr()
{
    bc95_serial_clearbuf();
    Serial.println(F("AT+NMGR"));
    bc95_serial.println(F("AT+NMGR"));
    bc95_serial_read();
}
             


/****************************************************************
 * desc: create udp socket 
 * * AT+NSOCR=<type>,<protocol>,<listen port>[,<recive control>]
 * * udp type:DGRAM, protocol:17, receive control default is 1 message will be receive
 * note: if a socket has already created, will fail if requested a second time
*************************************************************** */
void mcu_at_nsocr()
{
    bc95_serial_clearbuf();
    Serial.println(F("AT+NSOCR=DGRAM,17,5999,1")); 
    bc95_serial.println(F("AT+NSOCR=DGRAM,17,5999,1")); 
    bc95_serial_read();
}
             

/****************************************************************
 * desc: send data to udp server 
 * AT+NSOST=socket,remote_addr,remote_port, length, data
 * socket: number return by AT+NSOCR, 0 as default 
*************************************************************** */
void mcu_at_nsost(int len, char *data)
{
    int i;
    char imeihex[32] = {'\0'};
    char datahex[MAXLEN] = {'\0'};

    str2hex(imeihex, sizeof(imeihex), IMEI, strlen(IMEI));

    str2hex(datahex, sizeof(datahex), data, len);

    bc95_serial_clearbuf();
    snprintf(retmcu, sizeof(retmcu), "AT+NSOST=0,%s,%d,%d,%s3D%s", udpserver, port, len + strlen(IMEI) - 1, datahex, imeihex); 
    Serial.println(retmcu);
    bc95_serial.println(retmcu);
    bc95_serial_read();
}
             

/****************************************************************
 * desc: read the req_length characters of data from socket 
 *
*************************************************************** */
void mcu_at_nsorf()
{
        bc95_serial_clearbuf();
        Serial.println(F("AT+NSORF=0,512"));    // read 512 characters form socket 0
        bc95_serial.println(F("AT+NSORF=0,512"));    // read 512 characters form socket 0
        bc95_serial_read();
        if(strstr(retmcu, "+NSONMI:") != NULL) {  // +NSONMI: has message 
            bc95_serial.println(F("AT+NSORF=0,512"));
            bc95_serial_read();
        }
}

/**************************************************************************************
 * funciton name  ：mcu_at_cgpaddr()
 * parameter      ：*ip 
 * return value   ：NONE 
 * effect         ：send "AT+CGPADDR"command,display the PDP addr
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_cgpaddr()
{
    bc95_serial_clearbuf();
    Serial.println(F("AT+CGPADDR"));
    bc95_serial.println(F("AT+CGPADDR"));
    bc95_serial_read();
}
/**************************************************************************************
 * funciton name  ：mcu_at_nping()
 * parameter      ：*ip 
 * return value   ：NONE 
 * effect         ：ping
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_at_nping(char *ip)
{
    char cmd[64] = {'\0'};
    snprintf(cmd, sizeof(cmd), "AT+NPING=%s", ip);   //函数原型为int snprintf(char *str, size_t size, const char *format, ...)
    bc95_serial_clearbuf();
    Serial.println(cmd);
    bc95_serial.println(cmd);
}
/**************************************************************************************
 * funciton name  ：mcu_init()
 * parameter      ：NONE 
 * return value   ：NONE 
 * effect         ：init the NB-IoT module
 * data           ：2018/4/16
*****************************************************************************************/
void mcu_init()
{
    mcu_at_nband();      //choose the NB-IoT working band
    mcu_at_nconfig();    //config auto look for network

    if (reboot) {        
        reboot = 0;      //
        mcu_at_nrb();    //reboot the NB-IoT module
        delay(5000);     //delay 5 second
    }

    if (NULL == mcu_get_imei(IMEI))//if return value is null,Arduino Send char string to computer port
        Serial.println(F("IMEI = NULL"));
    else {
        Serial.print(F("IMEI="));
        Serial.println(IMEI);      //else Arduino send IMEI code to computer port
    }

    mcu_at_ncdp();      //query IP addr
    mcu_at_csq();       //query signal if strengh
    mcu_at_cgatt();     //Send "AT+CGATT" command 
    mcu_at_cereg();     //query network register satues is success,"1"resprent success
    mcu_at_cgdcont();   //quest work statues,second parameter is return statues,"1" represent is connet
    mcu_at_cgpaddr(); 
    mcu_at_nsocr();    // Greate a SOcket
}

/****************************************************************
 * desc : 
 *
*************************************************************** */
void send_data_udp(int len, char *data)
{
    mcu_at_nsost(len, data);
}

/****************************************************************
 * desc : 
 *
*************************************************************** */
void mcu_rec_data()
{
    mcu_at_nsorf();
}

