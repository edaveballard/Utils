#include <Servo.h>
#include <EEPROM.h>

#define  MAX_KEYS      32
#define  INITIALIZED   111
#define  LOC_INIT      0
#define  LOC_NIDS      1
#define  LOC_IDTBL     9
#define CHECK_INTERVAL 950 
#define ADD_INTERVAL   8000

Servo dservo;
boolean locked;
const int ledpin = 5;
boolean led = false;
const int spin = 6;
const int enpin = 2;
const int buttonpin = 8;
unsigned long timepoint = 0;
unsigned long ulocktimepoint = 0;
unsigned long buttontimepoint;
boolean button = false;
int numids;
int id[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

/*
in eeprom: 
byte 0 is whether eeprom has been initialized (111 if it has)
byte 1 is number of ids, set to 0 on initialization
byte 9 begins table of ids (12 bytes for each)
*/

void setup()
{
    dservo.attach(9);
    Serial.begin(2400);
    locked = false; 
    pinMode(ledpin,OUTPUT);
    digitalWrite(ledpin,LOW);
    pinMode(spin,OUTPUT);
    digitalWrite(spin,LOW);
    pinMode(enpin,OUTPUT);
    digitalWrite(enpin,LOW);
    pinMode(buttonpin,INPUT);
    
    if(EEPROM.read(LOC_INIT) != INITIALIZED)
    {
      EEPROM.write(LOC_INIT,INITIALIZED);
      EEPROM.write(LOC_NIDS,0);
      numids = 0;
    }
    else
      numids = EEPROM.read(LOC_NIDS);
    
}

void flush_buffer()
{
  while(Serial.available())
 {
  Serial.read();
 } 
}

void lock()
{
  digitalWrite(enpin,LOW);
  dservo.write(0);
  delay(600);
  dservo.write(90);
  locked = true;
}

void unlock()
{
  digitalWrite(enpin,HIGH);
  dservo.write(180);
  int t = 397;
  for(float i = 0; i < 600; i++)
  {
    digitalWrite(spin,HIGH);
    delayMicroseconds(t);
    digitalWrite(spin,LOW);
    delayMicroseconds(t);
  }
  t = 500;
  for(float i = 0; i < 600; i++)
  {
    digitalWrite(spin,HIGH);
    delayMicroseconds(t);
    digitalWrite(spin,LOW);
    delayMicroseconds(t);
  }
  dservo.write(90);
  locked = false;
  ulocktimepoint = millis();
}

void readid()
{
  //reads into id[]
  for(int k = 0; k < 12; k++)
  {
    int j = Serial.read();
    id[k] = j;
  }
 
  flush_buffer();
}

boolean validid()
{
  //checks id in id[] against list of ids, returns true if it exists
  for(int i = 0; i < numids; i++)
  {
    int addr = i * 12 + LOC_IDTBL;
    boolean match = true;
    for(int j = 0; j < 12; j++)
    {
      if(id[j] != EEPROM.read(addr + j))
      {
        match = false;
        break; 
      }
    }
    if(match)
      return true;
  }
  
  
  return false; 
}

boolean addid()
{
  //add id[] to list, if it doesn't exist already
 int addr = numids * 12 + LOC_IDTBL;
 if(validid() && numids < MAX_KEYS)
   return false;
 
 for(int i = 0; i < 12; i++)
 {
   EEPROM.write(addr + i,id[i]);
 }
 
 numids++;
 EEPROM.write(LOC_NIDS,numids);
 return true;
}

void checkforkey()
{
  if(!Serial.available())
    return;
  digitalWrite(ledpin,HIGH);
  readid();
  if(validid())
  {
    unlock();
    ulocktimepoint = millis();
  }
  digitalWrite(ledpin,LOW);
  flush_buffer();
}

void addmode()
{
  flush_buffer();
  while(!Serial.available())
  {
  
  if(abs(millis()-timepoint) > 500)
  {
     if(led)
     {
      led = false;
      digitalWrite(ledpin,LOW); 
     }
     else
     {
       led = true;
       digitalWrite(ledpin,HIGH);
     }
     timepoint = millis();
  }
  
  }
  digitalWrite(ledpin,LOW); 
  addid();
}


void loop()
{
  if(digitalRead(buttonpin) == HIGH)
  {
    if(button)
    {
      if(abs(millis()-buttontimepoint) > ADD_INTERVAL)
      {
        led = false;
        addmode();
      }
    }
    else
    {
      button = true;
      buttontimepoint = millis();
      if(locked)
      {
        unlock();
        ulocktimepoint = millis();
      }
      else
      {
        lock();
      }
    }
    
  }
  else
    button = false;
  
  
  
   if(locked && abs(millis()-timepoint) > CHECK_INTERVAL)
   {
    timepoint = millis();
    checkforkey(); 
   }
   if(!locked && abs(millis()-ulocktimepoint) > 30000)
   {
    lock(); 
   }
   
}
