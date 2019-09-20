#include "FastLED.h"
#define NUM_LEDS 53
#define SIZE NUM_LEDS - 1
#define DIN 6

// STATES OF STATE MACHINE
#define STATE_RAINBOW 0

// STATES OF MIX
#define MIX_NONE 0
#define MIX_F 1
#define MIX_B 2
#define MIX_STILL 3


short state = STATE_RAINBOW;

CRGB leds[NUM_LEDS];
CRGB buff[NUM_LEDS];

float offset = 0.0f;
float s = 53.0f / 850.0f;

// mix section
float mix = 1.0f;
int mix_still = 2000;
float mix_step = 0.01f;
short mix_state = MIX_B;

CRGB mix_color = CRGB::Black;

void setup() { 
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DIN>(leds, NUM_LEDS); 
  FastLED.show();
  
  FeelBuffAsRainbow(buff);
}

void loop() { 
  ProcessInput();
  ProcessMix();
  
  switch(state) {
    case STATE_RAINBOW:
      ProcessRainbow();
      break;
  }    

   FastLED.show();
   delay(10);
}

void ProcessMix() {
  switch(mix_state) {
    case MIX_F:
      // move forward
      if (mix >= 1.0f){
        mix = 1.0f;
        mix_state = MIX_STILL;
        break;
      }
    
      mix += mix_step;      
      break;
    case MIX_B:
      // move back
      if (mix <= 0.0f){
        mix_state = MIX_NONE;
        mix_color = CRGB::Black;
        mix = 0.0f;
        break;
      }
    
      mix -= mix_step; 
      break;
    case MIX_STILL:
      delay(mix_still);
      mix_state = MIX_B;
      break;
  }  
}

void ProcessInput() {
  if (Serial.available() > 0) {
    char arg = Serial.read();   
    if (arg == '1') {
       FadeFrom(CRGB::Red);
    }

    if (arg == '2') {
       FadeFrom(CRGB::Green);
    }

    if (arg == '3') {
       FadeFrom(CRGB::Aqua);
    }
  }
}

void FadeFrom(CRGB color){
  if (mix_state != MIX_NONE) {
    return;
  }
  
  mix_color = color;
  mix_state = MIX_F;
}

void ProcessRainbow() {
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
}

CRGB Extract(CRGB* arr, float pos){
  int idxFrom = floor(pos);  
  int idxTo = idxFrom + 1;

  CRGB colorFrom = arr[idxFrom];
  CRGB colorTo = arr[idxTo];

  float k = pos - idxFrom;  
  CRGB preColor =  Mix(colorFrom, colorTo, k);
  if (mix_state == MIX_NONE) {
    return preColor;
  }
  
  return Mix(preColor, mix_color, mix);
}

/* 
 *  color Mix from A to B. k is position between A and B (0..1)
 */
CRGB Mix(CRGB colorFrom, CRGB colorTo, float k) {
  if (k == 0.0f) {
    return colorFrom;
  }

  if (k == 1.0f) {
    return colorTo;
  }
  
    // extract colors 
  short r = ColorClamp((short)(colorFrom.r + (float)(colorTo.r - colorFrom.r) * k));
  short g = ColorClamp((short)(colorFrom.g + (float)(colorTo.g - colorFrom.g) * k));
  short b = ColorClamp((short)(colorFrom.b + (float)(colorTo.b - colorFrom.b) * k));
    
  return CRGB(r, g, b);
}

short ColorClamp(short value){
  if (value < 0) return 0;
  if (value > 255) return 255;
  return value; 
}

void FeelBuffAsRainbow(CRGB* arr){
  
  
  int sectionSizeBase = (int)((float)NUM_LEDS / 6.0f); 
  int sectionSize = sectionSizeBase;
  int section = 0;
  int secSP = 0; // section start position
  int secEP = sectionSize; // section end position

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
