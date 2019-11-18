//Unlocks the door if
#include <Servo.h>
#include <LiquidCrystal.h>

Servo dservo;
boolean locked;
int ledpin = 5;
int spin = 6;
int enpin = 2;
int buttonpin = 8;
LiquidCrystal lcd(3,4,10,11,12,13);

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
    lcd.begin(16, 2);
}

void lock()
{
  dservo.write(0);
  delay(600);
  dservo.write(90);
  locked = true;
}

boolean verifyit(int i1[])
{
  delay(400);
  if(!match(i1,readit()))
    return false;
 // lcd.print(i);
  lcd.clear();
  printit(i1);
  return true;
}

void unlock()
{
  dservo.write(180);
  int t = 500;
  for(float i = 0; i < 600; i++)
  {
    digitalWrite(spin,HIGH);
    delayMicroseconds(t);
    digitalWrite(spin,LOW);
    delayMicroseconds(t);
  }
  delay(300);
  dservo.write(90);
  locked = false;
}

int* readit()
{
  lcd.clear();
  int out[12];
    for(int k = 0; k < 12; k++)
    {
      int j = Serial.read();
      out[k] = j;
     // lcd.print(j,HEX);

    }
 //printit(out);
 Serial.flush();
 return out;
}

boolean match(int i1[], int i2[])
{
 for(int i = 0; i < 12; i++)
 {
   if(i1[i] != i2[i])
     return false;
 } 
 return true;
}

void printit(int i1[])
{
  for(int k = 0; k < 12; k++)
  {
   if(i1[k] < 16)
     lcd.print(0);
   lcd.print(i1[k],HEX);
   if(k == 5)
    lcd.setCursor(0, 1);
  }

}

void loop()
{
  if(!locked)
  {
   digitalWrite(enpin,HIGH);
   delay(15000); //waits 15 seconds to lock the door again.
   digitalWrite(enpin,LOW);
   lock();
  }
  
  
  //if it is locked
  delay(900);  //checks every .9 seconds
  if(digitalRead(buttonpin) == HIGH)
    unlock();
  //digitalWrite(enpin,LOW);
  //delay(300);
  if(Serial.available())
  {
    digitalWrite(ledpin,HIGH);
    int* id = readit();
    printit(id);
    //Here we would check if it were a recognized id!!
    //once I get my lcd screen....
    if(verifyit(id))
    {
      unlock();
    }
    digitalWrite(ledpin,LOW);
  }
  //digitalWrite(enpin,HIGH);

  Serial.flush();
}
