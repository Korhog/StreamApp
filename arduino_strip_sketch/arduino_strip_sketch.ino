#include "FastLED.h"
#define NUM_LEDS 53
#define SIZE NUM_LEDS - 1
#define DIN 6

CRGB leds[NUM_LEDS];
CRGB buff[NUM_LEDS];

float offset = 0.0f;
float s = 53.0f / 350.0f;

void setup() { 
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DIN>(leds, NUM_LEDS); 
  FastLED.show();
  
  FeelBuffAsRainbow(buff);
  Feel(CRGB::Red);
  delay(1000);
}

void loop() {
  // 
  offset += s;
  if (offset >= SIZE)
    offset -= SIZE;
    
  for (int i = 0; i < NUM_LEDS; i++) {
    float pos = i + offset;
    if (pos >= SIZE) {
      pos -= SIZE;
    }

    leds[i] = Extract(buff, pos);    
  }
   FastLED.show();
   delay(10);
}

void Feel(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void Blink(CRGB color) {
  Feel(color);
  delay(500);
  Feel(CRGB::Black);
}

CRGB Extract(CRGB* arr, float pos){
  int idxFrom = floor(pos);  
  int idxTo = idxFrom + 1;

  CRGB colorFrom = arr[idxFrom];
  CRGB colorTo = arr[idxTo];

  float k = pos - idxFrom;  
  return Mix(colorFrom, colorTo, k);
}

/* 
 *  color Mix from A to B. k is position between A and B (0..1)
 */
CRGB Mix(CRGB colorFrom, CRGB colorTo, float k) {
    // extract colors 
  short r = (short)(colorFrom.r + (float)(colorTo.r - colorFrom.r) * k);
  short g = (short)(colorFrom.g + (float)(colorTo.g - colorFrom.g) * k);
  short b = (short)(colorFrom.b + (float)(colorTo.b - colorFrom.b) * k);
  
  return CRGB(r, g, b);  
}

void FeelBuffAsRainbow(CRGB* arr){
  
  
  int sectionSizeBase = (int)((float)NUM_LEDS / 6.0f); 
  int sectionSize = sectionSizeBase;
  int section = 0;
  int secSP = 0; // section start position
  int secEP = sectionSize; // section end position

  Serial.print(sectionSize);

  CRGB colorFrom = CRGB::Green;
  CRGB colorTo = CRGB::Red;

  double x = 1.0 / (double)sectionSize;
  
  for (int i = 0; i < NUM_LEDS; i++) { 
    section = i * x; // section num;  
    

    
    secSP = section * sectionSizeBase;
    secEP = secSP + sectionSizeBase;
    if (section == 6) {      
      secEP = SIZE;  
      sectionSize = SIZE - secSP; 
    }

    RainbowSection(section, colorFrom, colorTo);
    float k = (float)(i - secSP) / (float)sectionSize;
    arr[i] = Mix(colorFrom, colorTo, k);       
  }
}

void RainbowSection(int section, CRGB &a, CRGB &b){
  switch(section){
    case 0: 
      a = CRGB::Red;  
      b = CRGB::Orange;
      break; 
    case 1: 
      a = CRGB::Orange;  
      b = CRGB::Yellow;
      break; 
    case 2: 
      a = CRGB::Yellow;  
      b = CRGB::Green;
      break;
    case 3: 
      a = CRGB::Green;  
      b = CRGB::Aqua;
      break;  
    case 4: 
      a = CRGB::Aqua;  
      b = CRGB::Blue;
      break;
    case 5: 
      a = CRGB::Blue;  
      b = CRGB::Purple;
      break;
    case 6: 
      a = CRGB::Purple;  
      b = CRGB::Red;
      break;        
  }
}
