#include <SoftwareSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include<dht.h>
#define baud 9600
#define DHT11_PIN A0

dht DHT;

float temperature,tem,humidity,hum;        //parameter

/******define software Serial port*******/
const byte bg96_rx = 10;
const byte bg96_tx = 11;

/******define the udp parameter const*******/
const char UDP_server[] = "120.78.138.177";
const int UDP_port = 7778;
/******define the tcp parameter const*******/
const char TCP_server[] = "120.78.138.177";
const int TCP_port = 8887;

static char return_dat[256] = {"\0"};   //save the UE return all data

SoftwareSerial bg96_serial(bg96_rx, bg96_tx);
void setup() {
  bg96_initial();
}

void loop() {
  read_dht();
  udp_protocal();
  tcp_protocal();
  delay(600000);
}
void bg96_initial(){
  delay(1000);
  Serial.begin(baud);            
  bg96_serial.begin(baud);                 //open the Serial ,baud is 9600bps
  Serial.println("start the serial port,baud is 9600bps.");
  delay(500);
  bg96_at_cmd();
  delay(500);
  bg96_ate_cmd();
  delay(500);
  bg96_at_cimi();
  bg96_at_csq();
  delay(500);
  bg96_at_cereg();
  delay(500);
  bg96_at_cgpaddr();
  delay(500);
  //tcp_config();
  tcp_close();
  delay(500);
  tcp_config();
  }
 /***********************
 * name:bg96_serial_read()
 * data:2018/7/23
 * para:none
 * bref:read the bg96 return data
 */
  void read_dht()
{                
    temperature = DHT.read11(DHT11_PIN);    //Read Tmperature data
    tem = DHT.temperature*1.0;             
    humidity = DHT.read11(DHT11_PIN);      //Read humidity data
    hum = DHT.humidity* 1.0;  
    Serial.println("the Tmperature is:");
    Serial.println(tem);                 //print tem value
    Serial.println("the Humidity is:");
    Serial.println(hum);                 //print hum value
 }
/***********************
 * name:bg96_serial_read()
 * data:2018/7/23
 * para:none
 * bref:read the bg96 return data
 */
void bg96_serial_read()
{
  int i = 0;
  memset(return_dat, 0, sizeof(return_dat));
  bg96_serial.flush();
  delay(2000);
  while (bg96_serial.available() && i < sizeof(return_dat) - 1)
  {
    return_dat[i] = bg96_serial.read();
    i = i + 1;
  }
  return_dat[i] = '\0';
  Serial.println(return_dat);
}
/***********************
 * name:bg96_serial_clearbuf()
 * data:2018/7/23
 * para:none
 * bref:clear the buffer data
 */
void bg96_serial_clearbuf()
{
  bg96_serial.flush();

  while (bg96_serial.available()) {
    bg96_serial.read();
  }
}
/********tcp protocol*********/
void tcp_config(){
  start_tcp();
  }
void start_tcp(){
  char cmd[64]={"0"};
  snprintf(cmd,sizeof(cmd),"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0",TCP_server,TCP_port);
  while(1){
  bg96_serial_clearbuf();
  Serial.println(cmd);
  bg96_serial.println(cmd);
  bg96_serial_read();
  if(strstr(return_dat,"OK")!=NULL){
    Serial.println("the tcp already connect");
    break;}
  }
  }
/********tcp protocol*********/
void tcp_protocal()
{
  tcp_send_dat();
  delay(3000);
  tcp_receive_dat();
  }
void tcp_close()
{
  Serial.println("close tcp server.");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QICLOSE=0");
  bg96_serial_read();
  }
void tcp_send_dat()
{
  char cmd[64]={"0"};
  int tem_v,hum_v;
  int tem_t,tem_u,tem_l1,tem_l2;
  int hum_t,hum_u,hum_l1,hum_l2;
  tem_v = (int)(tem*100);
  hum_v = (int)(hum*100);
  tem_t = tem_v/1000+30;
  tem_u = tem_v%1000/100+30;
  tem_l1 = tem_v%1000%100/10+30;
  tem_l2 = tem_v%1000%100/10%10+30;
  
  hum_t = hum_v/1000+30;
  hum_u = hum_v%1000/100+30;
  hum_l1 = hum_v%1000%100/10+30;
  hum_l2 = hum_v%1000%100/10%10+30;           //deal with the data 
  snprintf(cmd,sizeof(cmd),"AT+QISENDEX=0,\"%d%d2E%d%d2020%d%d2E%d%d\"",tem_t,tem_u,tem_l1,tem_l1,hum_t,hum_u,hum_l1,hum_l2);
  Serial.println("prinln the tcp send data");
  bg96_serial.println(cmd);
  Serial.println(cmd);
  }
void tcp_receive_dat()
{
  Serial.println("receive TCP data:");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QIRD=0,1500");
  bg96_serial_read();
  }
/********udp protocol*********/
void udp_protocal(){
  start_udp();
  udp_hardware();
  udp_send();
  udp_recieve();
  udp_close();
}
void snprintf_fun1(){
  char cmd[64]={"0"};
  snprintf(cmd,sizeof(cmd),"AT+QIOPEN=1,2,\"UDP SERVICE\",\"%s\",0,%d,0",UDP_server,UDP_port);
  Serial.println(cmd);
  bg96_serial.println(cmd);
  }
void start_udp()
{
    Serial.println("Start a UDP Service.");
  while(1){
  bg96_serial_clearbuf();
  snprintf_fun1();
  bg96_serial_read();
  if(strstr(return_dat,"+QIOPEN: 2,0")){
    Serial.println("the udp server already set up");
    break;}
   else{
    while(1){
    Serial.println("close the UDP service");
    bg96_serial_clearbuf();
    bg96_serial.println("AT+QICLOSE=2");
    bg96_serial_read();
    if(strstr(return_dat,"OK")!=NULL)
    break;
    }
    }
  }
  }
void udp_hardware(){
   bg96_serial_clearbuf();
   bg96_serial.println("AT+QISTATE=0,1");
   bg96_serial_read();
  }

void udp_send()
{
  char cmd[64]={"0"};
  char cmd1[64]={"0"};
  int tem1=0;
  int hum1=0;
  int tem_h=0;
  int tem_l=0;
  int hum_h=0;
  int hum_l=0;
  tem1 = (int)(tem*100);
  hum1 = (int)(hum*100);

  tem_h=tem1/100;
  tem_l=tem1%100;

  hum_h=hum1/100;
  hum_l=hum1%100;                                   //temperature and humidity data deal with
  
  bg96_serial_clearbuf();
  Serial.println("Send UDP Data to Remote");
  snprintf(cmd1,sizeof(cmd1),"dragino-->tem=%d.%d,hum=%d.%d",tem_h,tem_l,hum_h,hum_l);
  snprintf(cmd,sizeof(cmd),"AT+QISEND=2,%d,\"%s\",%d",(strlen(cmd1)+1),UDP_server,UDP_port);
  Serial.println(cmd);
  bg96_serial.println(cmd);
  bg96_serial_read();
  if(strstr(return_dat,">")!=NULL){
  bg96_serial.println(cmd1);
    }
  bg96_serial_read();
  }
void udp_recieve()
{
 delay(2000);
 Serial.println("receive the UDP data:");
 bg96_serial_clearbuf();
 bg96_serial.println("AT+QIRD=2");
 bg96_serial_read();
  }
void udp_close()
{
  Serial.println("close the udp service");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QICLOSE=2");
  bg96_serial_read();
  }
/***********************
 * name:bg96_at_cmd()
 * data:2018/7/23
 * para:none                return_dat
 * bref:detect the module can carry
 */
 void bg96_at_cmd(){
  Serial.println("\n");
  Serial.println("send the at command,wait for return \"OK\"");
  while(1){
    bg96_serial_clearbuf();//clear the return data buffer
    bg96_serial.println("AT");
    bg96_serial_read();
    if(strstr(return_dat,"OK") != NULL){
      Serial.println("<----------------------->");
      break;}
    else 
      delay(1000);
    }}
/***********************
 * name:bg96_ate_cmd
 * data:2018/7/23
 * para:none                return_dat
 * bref:close the return display
 */
 void bg96_ate_cmd(){
  Serial.println("close the return display");
  while(1){
    bg96_serial_clearbuf();//clear the return data buffer
    bg96_serial.println("ATE0");
    bg96_serial_read();
    if(strstr(return_dat,"OK") != NULL){
      Serial.println("<----------------------->");
      break;}
    else 
      delay(1000);
    }}  
/***********************
 * name:bg96_ate_csq
 * data:2018/7/23
 * para:none                
 * bref:look for the signal strenght
 */ 
void bg96_at_cimi(){
  Serial.println("sent the \"AT+CIMI\" command");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+CIMI");
  bg96_serial_read();
  delay(1000);
  Serial.println("sent the \"AT+CGSN\" command");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+CGSN");
  bg96_serial_read();
  delay(1000);
  }
void bg96_at_csq(){
  Serial.println("\n");
  Serial.println("the module look for the signal strenght:");
  while(1){
    bg96_serial_clearbuf();
    bg96_serial.println("AT+CSQ");
    bg96_serial_read();
   // if(strstr(return_dat,"+CSQ: 99,99") != NULL){
    //  Serial.println("config the NB modlue");
      //bianixe cheng xu
    // }
      if(strstr(return_dat,"+CSQ: 99,99") != NULL){
      bg96_serial.println("AT+QCFG=\"nwscanmode\",0,1");
      Serial.println("AT+QCFG=\"nwscanmode\",0,1");
      Serial.println("wait for 5 second");
      delay(5000);
      }
    else{
      Serial.println("the module has signal");
      break;}
    }
  }
void bg96_at_cereg()
{
  Serial.println("\n");
  Serial.println("look for network already register.");
  while(1){
    bg96_serial_clearbuf();
    bg96_serial.println("AT+CEREG?");
    bg96_serial_read();
    if(strstr(return_dat,"+CEREG: 0,1") != NULL)
    {
      Serial.println("the module already register");
      break;
      }
    else{
      //信号处理
      delay(5000);
      }
  }
  }
void bg96_at_cgpaddr(){
  Serial.println("\n");
  Serial.println("look for the temporary IP");
    bg96_serial_clearbuf();
    bg96_serial.println("AT+CGPADDR");
    bg96_serial_read();
}
