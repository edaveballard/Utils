#include <Adafruit_NeoPixel.h>
#include <math.h>

#define LED_COUNT 17
#define CENTR 8
#define REED_PIN 23

//frequency in hertz
float freq = 4;
const int r_res = 10;
const int th_res = 90;
unsigned long last_time;
boolean last_on = 0;

unsigned long mwait = long(1000000/(freq*th_res));
const unsigned char pins[] = {4,6};
Adafruit_NeoPixel leds[sizeof(pins)/sizeof(char)];

float th_step = 360/th_res;

float angle;

#define c1 0x222200
#define c2 0x002200
#define c3 0x000022
#define c4 0x222222
#define c5 0x220000

float img[][4] = {
                  {5,-4,1,c1},{5,-3,1,c1},{5,-2,1,c1},{5,-1,1,c1},{5,0,1,c1},{5,1,1,c1},{5,2,1,c1},{5,3,1,c1},{5,4,1,c1},{5,5,1,c1},
                  {-5,-4,1,c2},{-5,-3,1,c2},{-5,-2,1,c2},{-5,-1,1,c2},{-5,0,1,c2},{-5,1,1,c2},{-5,2,1,c2},{-5,3,1,c2},{-5,4,1,c2},
                  {-5,-5,1,c3},{-4,-5,1,c3},{-3,-5,1,c3},{-2,-5,1,c3},{-1,-5,1,c3},{0,-5,1,c3},{1,-5,1,c3},{2,-5,1,c3},{3,-5,1,c3},{4,-5,1,c3},
                  {-5,5,1,c4},{-4,5,1,c4},{-3,5,1,c4},{-2,5,1,c4},{-1,5,1,c4},{0,5,1,c4},{1,5,1,c4},{2,5,1,c4},{3,5,1,c4},{4,5,1,c4},

                  {-7,-5,0,c5},{-6,-5,0,c5},{-5,-5,0,c5},{-4,-5,0,c5},{-3,-5,0,c5},{-2,-5,0,c5},{-1,-4,0,c5},{-1,-3,0,c5},{-7,-2,0,c5},{-6,-2,0,c5},{-5,-2,0,c5},{-4,-2,0,c5},{-3,-2,0,c5},{-2,-2,0,c5},
                  {-7,2,0,c5},{-6,2,0,c5},{-5,2,0,c5},{-4,2,0,c5},{-3,2,0,c5},{-2,2,0,c5},{-1,3,0,c5},{-4,4,0,c5},{-3,4,0,c5},{-2,4,0,c5},{-1,5,0,c5},{-7,6,0,c5},{-6,6,0,c5},{-5,6,0,c5},{-4,6,0,c5},{-3,6,0,c5},{-2,6,0,c5},
                  {7,-5,0,c5},{6,-5,0,c5},{5,-5,0,c5},{4,-5,0,c5},{3,-5,0,c5},{2,-5,0,c5},{2,-4,0,c5},{2,-3,0,c5},{2,-2,0,c5},{3,-2,0,c5},{4,-2,0,c5},{5,-2,0,c5},{6
                  ,-2,0,c5},
                  {7,2,0,c5},{6,2,0,c5},{5,2,0,c5},{4,2,0,c5},{3,2,0,c5},{2,2,0,c5},{2,3,0,c5},{2,4,0,c5},{2,5,0,c5},{5,3,0,c5},{5,4,0,c5},{5,5,0,c5}
                  };
                 

//float img[][3] = {{4,-4,c1},{4,4,c2},{-4,4,c3},{-4,-4,c4}};

void rect2polar(float* rect) {
  float x = rect[0];
  float y = rect[1];

  float r = sqrt(x*x+y*y);
  float th = atan(y/x)*180/M_PI;

  if(x < 0) {
    th += 180;
  }

  if(th < 0) {
    th += 360;
  } else if(th > 360) {
    th -= 360;
  }
  rect[0] = r;
  rect[1] = th;
}

void setup() {
  // put your setup code here, to run once:

  last_time = micros();
  Serial.begin(9600);
  //image (list of points in Cartesian coordinates)
  //convert image to polar coordinates
  for (int i = 0; i < sizeof(img)/sizeof(float[4]); i++ ) {
    rect2polar(img[i]);
  }

   

  angle = 0;
  for(int i = 0; i < sizeof(pins)/sizeof(char); i++) {
    leds[i] = Adafruit_NeoPixel(LED_COUNT, pins[i], NEO_GRB + NEO_KHZ800);
    leds[i].begin();
    leds[i].show();
  }

  pinMode(REED_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REED_PIN), zeroangle, FALLING);
  
}

void loop() {
  //int zero = digitalRead(REED_PIN);
  unsigned long mstart=micros();

  
  angle += th_step;
  if(angle >= 360) {
    angle = 0;
  }

  float opp_ang = angle - 180;
  if(opp_ang < 0) {
    opp_ang += 360;
  }

/*
  if (zero == LOW){
    //leds[0].setPixelColor(0, c1);
    //prevent issues with wind-up, debounce
    unsigned long current_time = micros();
    unsigned long mperiod = current_time - last_time;
    //if(mperiod > long(1000000/(1*th_res)) || mperiod < long(1000000/(20*th_res))) {
    Serial.print(mperiod);
    Serial.print(" ");
    Serial.println(mwait);
    if(mperiod > 200000) {
      mperiod = mwait;
    }
    if(last_on == 1 || mperiod <= long(1000000/(20*th_res))){
      //mperiod = mwait;
    } else {
      
    }
    last_time = current_time;
    angle = 0;
    freq = 1000000/mperiod;
    mwait = (mwait+mperiod)/2;

    last_on = 1;
  } else {
    last_on = 0;
  }*/
  /*if (zero == LOW){
    angle = 0;
  }*/

  
  //turn off all lights
  for (int j = 0; j < sizeof(pins)/sizeof(char); j++) {
    leds[j].clear();
  for (int i = 0; i < sizeof(img)/sizeof(float[4]); i++ ) {
    if(int(img[i][2]) == j){
    int light = int(img[i][0]+0.5);
    if( abs(img[i][1] - angle ) < th_step*2/4 ) {
      //turn on appropriate led
      leds[int(img[i][2])].setPixelColor(CENTR-light, img[i][3]);
      //Serial.println(i);

    } else if( abs(img[i][1] - opp_ang ) < th_step*2/4 ) {
      
      leds[int(img[i][2])].setPixelColor(CENTR+light, img[i][3]);
    }
    }
  }
    leds[j].show();
  }

  delayMicroseconds(mwait-(micros()-mstart));
}


void zeroangle() {
  unsigned long current_time = micros();
  unsigned long mperiod = current_time - last_time;
  last_time = current_time;
  angle = 0;
  //freq = 1000000/mperiod;
  if(abs(100*(mwait-mperiod)/mwait) < 50) {
    mwait = (mwait+mperiod)/2;
  }

  
}

