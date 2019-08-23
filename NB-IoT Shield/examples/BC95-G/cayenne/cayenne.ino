#include "common.h"
#include <dht.h>

//#define activation
#define DHT11_PIN A0

NB nb;
dht DHT;

uint16_t num = 0;
float tem,hum;
unsigned long old_time=0,new_time=0;
long time = 180000;

void setup() {
  Serial.begin(9600);
  nb.serialInit();
  nb.deviceInit(); 
  
  #ifdef activation
    while(!nb.conCayenne()); Serial.println("Connected to the cayenne.");
    while(!nb.conDevice()){
      return;
    }Serial.println("Device activated!");
    nb.closeMqtt();
  #endif
}

void loop() {
  #ifndef activation
  new_time = millis();
  if (new_time - old_time >= time || num == 0)
  {
    Serial.println("---------Text Start:" + String(num) + "--------");
    while(!nb.conCayenne()); Serial.println("Connected to the cayenne.");
    while(!nb.conDevice()){
      return;
    }Serial.println("Connected to the device");
    dhtTem();
    nb.sendCayenne(tem,1);
    nb.sendCayenne(hum,2);
    Serial.println("Sent successly.");
    nb.closeMqtt();
    old_time = new_time;
    num ++;
    Serial.println("------------Text End---------" );
  }

  #endif
}

void dhtTem()
{
       DHT.read11(DHT11_PIN);   
       tem = DHT.temperature*1.0;      
       hum = DHT.humidity* 1.0; 
       Serial.println("The temperature and humidity:");
       Serial.print("[");
       Serial.print(tem);
       Serial.print("℃");
       Serial.print(",");
       Serial.print(hum);
       Serial.print("%");
       Serial.println("]");
}
