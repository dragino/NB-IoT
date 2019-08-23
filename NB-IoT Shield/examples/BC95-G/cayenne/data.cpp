/****Handling data used to connect to the platform****/

#include "common.h"

uint8_t* DATA::conDataProcessing()
{
  memset(DATA, 0, sizeof DATA);
  memset(con, 0, sizeof con);
  strcat(DATA,"AT+NSOSD=1,");
  uint16_t _clientID = strlen(clientID);
  uint16_t _userName = strlen(userName);
  uint16_t _passWord = strlen(passWord);

  uint16_t packetLen = 13; 
  if(_clientID > 0)
    packetLen = packetLen + 2 + _clientID;
  if(_userName > 0)
    packetLen = packetLen + 2 + _userName;
  if(_passWord > 0)
    packetLen = packetLen + 2 + _passWord; 
    
  itoa(packetLen,con, 10);
  strcat(DATA,con);
  strcat(DATA,",10");
  itoa(packetLen-3,con, 16);
  strcat(DATA,con);
  strcat(DATA,"0100044d51545404c2003c");

  if(_clientID < 16){
    itoa(_clientID,con, 16);
    strcat(DATA,"000");
    strcat(DATA,con);
  }else{
    itoa(_clientID,con, 16);
    strcat(DATA,"00");
    strcat(DATA,con);
  }
  for(int i=0;i<_clientID;i++)
  { 
    sprintf(con,"%x",clientID[i]);
    strcat(DATA,con);
  }

  if(_userName < 16){
    itoa(_userName,con, 16);
    strcat(DATA,"000");
    strcat(DATA,con);
  }else{
    itoa(_userName,con, 16);
    strcat(DATA,"00");
    strcat(DATA,con);
  }
  for(int i=0;i<_userName;i++)
  { 
    sprintf(con,"%x",userName[i]);
    strcat(DATA,con);
  }

  if(_passWord < 16){
    itoa(_passWord,con, 16);
    strcat(DATA,"000");
    strcat(DATA,con);
  }else{
    itoa(_passWord,con, 16);
    strcat(DATA,"00");
    strcat(DATA,con);
  }
  for(int i=0;i<_passWord;i++)
  { 
    sprintf(con,"%x",passWord[i]);
    strcat(DATA,con);
  }
  strcat(DATA,"\n");
  return DATA;
}

uint8_t* DATA::sendDataProcessing(float val,int channel)
{
  char buff[5];
  char buff1[5];
  String str1 = String (val);
  String str2 = String (channel);
  memset(DATA, 0, sizeof DATA);
  memset(con, 0, sizeof con); 
  strcat(DATA,"AT+NSOSD=1,"); 

  uint16_t _clientID = strlen(clientID);
  uint16_t _userName = strlen(userName);
  uint16_t _channel = str2.length();
  uint16_t _val = str1.length();
  
  uint16_t packetLen = 30; 
  if(_clientID > 0)
    packetLen = packetLen +_clientID;
  if(_userName > 0)
    packetLen = packetLen + _userName;
  if(_channel > 0)
    packetLen = packetLen + _channel;
  if(_val > 0)
    packetLen = packetLen + _val;
  
  itoa(packetLen,con, 10);
  strcat(DATA,con);
  strcat(DATA,",32");
  itoa(packetLen-2,con, 16);
  strcat(DATA,con);
  if((_clientID + _userName + _channel + 17) < 255){
    strcat(DATA,"00");
    itoa(_clientID + _userName + 18,con, 16);
    strcat(DATA,con);
  }

  strcat(DATA,"76312f");
  for(int i=0;i<_userName;i++)
  { 
    sprintf(con,"%x",userName[i]);
    strcat(DATA,con);
  }

  strcat(DATA,"2f7468696e67732f");
  for(int i=0;i<_clientID;i++)
  { 
    sprintf(con,"%x",clientID[i]);
    strcat(DATA,con);
  }
  strcat(DATA,"2f646174612f");
  
  strcpy(buff1,str2.c_str());
  for(int i=0;i<strlen(buff1);i++)
  {
    sprintf(con,"%x",buff1[i]);
    strcat(DATA,con);
  }
  strcat(DATA,"0001747970652c633d");
  
  strcpy(buff,str1.c_str());
  for(int i=0;i<strlen(buff);i++)
  { 
    sprintf(con,"%x",buff[i]);
    strcat(DATA,con);
  }
  
  strcat(DATA,"\n");
  return DATA;
}
