#include "common.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX,TX

void NB::serialInit(long baud)
{
    mySerial.begin(baud);
}

void NB::deviceInit()
{
    //...
    dataTransmission(BC95_NPSMR_ON);
    dataTransmission(BC95_CSCON_ON);
}

String NB::dataTransmission(String AT)
{
  String val = "";  
  while(mySerial.read() >= 0);
  mySerial.print(AT);
  while(mySerial.available() == 0); //Waiting for NB response
  while(mySerial.available() > 0){
    val = mySerial.readString();
  }
  val.trim();
//  Serial.println("-----------------------------");
//  Serial.println(val);
//  Serial.println("-----------------------------");
  return val;
}

bool NB::conCayenne()
{
  dataTransmission(BC95_NSOCL);
  
  faultCount = 0;
  while(dataTransmission(BC95_NSOCR) == "ERROR"){
     delay(1000);
     faultCount ++;
     if(faultCount > 20)
          return false;
  }
  faultCount = 0;
  while(dataTransmission(BC95_NSOCO) == "ERROR"){
     Serial.println("Waiting for the connection platform.");
     delay(1000);
     faultCount ++;
     if(faultCount > 20)
          return false;
  }

  faultCount = 0;
  while(dataTransmission(BC95_CSCON).substring(9,10) != "1"){  //Waiting for sleep mode to wake up
      Serial.println("Waiting module attached to the net.");
      delay(1000);
      faultCount ++;
      if(faultCount > 60)
          return F; 
    }
  return T;
}

bool NB::conDevice()
{
  faultCount = 0;
  do{  
     Serial.println("Waiting for connected device.");
     dataTransmission(conDataProcessing());
     delay(1000);
     faultCount ++;
     if(faultCount > 20){
      return F;
     }
  }while(dataTransmission(BC95_NSORF).length() < 10); 
  return T;
}

void NB::sendCayenne(float _val,int _channel)
{
  faultCount = 0;
  do{
     dataTransmission(sendDataProcessing(_val,_channel));
     delay(1000);
     faultCount ++;
     if(faultCount > 3){
      Serial.println("Sent fail.");
      return;
     }
  }while(dataTransmission(BC95_NSORF).length() < 10);
  //Serial.println("Sent successly.");
}

void NB::closeMqtt()
{
  dataTransmission(mqtt_end);
}

void NB::Text()
{

}
