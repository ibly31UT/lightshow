#include <Adafruit_NeoPixel.h>

#define DI_PIN 4
#define SENSOR_PIN 0
#define MAX_LED_COUNT 150
#define PI 3.14159265
#define PI_2 PI / 2

enum OP_MODE_ENUMS {
  OP_BOUNCE = 0,
  OP_RAINBOW,
  OP_FLOW,
  OP_FLOW2,
  OP_RANDOM,
  OP_RAINFLOW,
  OP_FILL,
  OP_MODE_COUNT
};

struct op_mode {
  int ledCount = 0;
  int msDelay = 0;
};

struct rgb_value {
  int r = 0;
  int g = 0;
  int b = 0;
};

struct rgb_value rgb(int r, int g, int b){
  struct rgb_value ret;
  ret.r = r;
  ret.g = g;
  ret.b = b;
  return ret;
}

struct op_mode op_modes[OP_MODE_COUNT];
int op_mode;

Adafruit_NeoPixel leds = Adafruit_NeoPixel(MAX_LED_COUNT, DI_PIN, NEO_GRB + NEO_KHZ800);
int intensity1[MAX_LED_COUNT];
int intensity2[MAX_LED_COUNT];
int current = 0;

float circle = 0.0f;
float counter = 0.0f;
int lastYPos = 0;

void changeOpMode(int op){
  for(int i = 0; i < op_modes[op_mode].ledCount; i++){
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();
  
  op_mode = op;
  leds = Adafruit_NeoPixel(MAX_LED_COUNT, DI_PIN, NEO_GRB + NEO_KHZ800);
  leds.begin();
  for(int i = 0; i < op_modes[op].ledCount; i++){
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();

  for(int i = 0; i < MAX_LED_COUNT; i++){
    intensity1[i] = 0;
    intensity2[i] = 0;
  }
}

struct rgb_value rainbow(float progress){
  progress = (abs(progress) - (int)abs(progress)) * 6.0f; // normalize
  int ascending = (int)((progress - (int)progress) * 255);
  int descending = 255 - ascending;

  switch((int)progress){
    case 0:
      return rgb(255, ascending, 0);
    case 1:
      return rgb(descending, 255, 0);
    case 2:
      return rgb(0, 255, ascending);
    case 3:
      return rgb(0, descending, 255);
    case 4:
      return rgb(ascending, 0, 255);
    default:
      return rgb(255, 0, descending);
  }
}

void setup() {
  leds.begin();
  for(int i = 0; i < MAX_LED_COUNT; i++){
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();
  
  op_modes[OP_BOUNCE].ledCount = 150;
  op_modes[OP_BOUNCE].msDelay = 8;

  op_modes[OP_RAINBOW].ledCount = 75;
  op_modes[OP_RAINBOW].msDelay = 4;

  op_modes[OP_FLOW].ledCount = 150;
  op_modes[OP_FLOW].msDelay = 45;

  op_modes[OP_FLOW2].ledCount = 150;
  op_modes[OP_FLOW2].msDelay = 40;
  
  op_modes[OP_RANDOM].ledCount = 75;
  op_modes[OP_RANDOM].msDelay = 50;

  op_modes[OP_RAINFLOW].ledCount = 75;
  op_modes[OP_RAINFLOW].msDelay = 2;

  op_modes[OP_FILL].ledCount = 75;
  op_modes[OP_FILL].msDelay = 4;
  
  changeOpMode(OP_RAINFLOW);

  Serial.begin(19200);
}

void loop() {
  int brightness = analogRead(0);
  leds.setBrightness(brightness / 4);
  
  /*Serial.println(Serial.available());
  if(Serial.available() >= 20){
    int i = 0;
    char message[20];
    while(i < 20)
      message[i++] = Serial.read();

    int mode = message[0] - '0';
    Serial.println(message);
    changeOpMode(mode);
  }*/
  if(Serial.available() > 0){
    int mode = Serial.read();
    mode = mode - '0';
    changeOpMode(mode);
  }
  
  int LED_COUNT = op_modes[op_mode].ledCount;   
  int DELAY = op_modes[op_mode].msDelay;
  int CENTER_OFFSET = 0; //(MAX_LED_COUNT - LED_COUNT) / 2;
  
  if(op_mode == OP_BOUNCE){
    for(int i = 0; i < LED_COUNT; i++){
      intensity1[i] = 260; 
      intensity2[LED_COUNT - i - 1] = 260;
      for(int j = 0; j < LED_COUNT; j++){
        intensity1[j] = max(intensity1[j] - 8, 0);
        intensity2[j] = max(intensity2[j] - 8, 0);
        
        leds.setPixelColor(CENTER_OFFSET + j, intensity1[j], 0, intensity2[j]);
      }
      leds.show();
      delay(DELAY);
    }
    for(int i = LED_COUNT; i >= 0; i--){
      intensity1[i] = 260; 
      intensity2[LED_COUNT - i - 1] = 260;
      for(int j = 0; j < LED_COUNT; j++){
        intensity1[j] = max(intensity1[j] - 8, 0);
        intensity2[j] = max(intensity2[j] - 8, 0);
        
        leds.setPixelColor(CENTER_OFFSET + j, intensity1[j], 0, intensity2[j]);
      }
      leds.show();
      delay(DELAY);
    }
  }else if(op_mode == OP_RAINBOW){
    struct rgb_value r = rainbow(counter);
    for(int i = 0; i < LED_COUNT; i++){
      leds.setPixelColor(CENTER_OFFSET + i, r.r, r.g, r.b);
    }
    counter += 0.0025f;
    leds.show();
    delay(DELAY);
  }else if(op_mode == 99){
    for(int i = 0; i < LED_COUNT; i++){
      for(int j = 0; j < LED_COUNT; j++){
        if(i < j)
          leds.setPixelColor(CENTER_OFFSET + j, 0, 255, 0);
        else if(i == j)
          leds.setPixelColor(CENTER_OFFSET + j, 255, 0, 0);
        else
          leds.setPixelColor(CENTER_OFFSET + j, 0, 0, 255);
      }
      leds.show();
      delay(DELAY);
    }
    for(int i = LED_COUNT; i >= 0; i--){
      for(int j = 0; j < LED_COUNT; j++){
        if(i < j)
          leds.setPixelColor(CENTER_OFFSET + j, 0, 255, 0);
        else if(i == j)
          leds.setPixelColor(CENTER_OFFSET + j, 255, 0, 0);
        else
          leds.setPixelColor(CENTER_OFFSET + j, 0, 0, 255);
      }
      leds.show();
      delay(DELAY);
    }
  }else if(op_mode == OP_FLOW){
    struct rgb_value r = rainbow(counter);
    for(int f = 0; f < 3; f++){
      for(int i = 0; i < LED_COUNT; i++){
        if(i % 3 == f)
          leds.setPixelColor(CENTER_OFFSET + i, r.r, r.g, r.b);
        else
          leds.setPixelColor(CENTER_OFFSET + i, 0, 0, 0);
      }
      counter += 0.005f;
      leds.show();
      delay(DELAY);   
    }
  }else if(op_mode == OP_FLOW2){
    for(int f = 0; f < 6; f++){
      for(int i = 0; i < LED_COUNT; i++){
        if(i % 6 == f)
          leds.setPixelColor(CENTER_OFFSET + i, 0, 0, 255);
        else if(i % 6 == (f + 1) % 6)
          leds.setPixelColor(CENTER_OFFSET + i, 0, 255, 0);
        else if(i % 6 == (f + 2) % 6)
          leds.setPixelColor(CENTER_OFFSET + i, 255, 0, 0);
        else
          leds.setPixelColor(CENTER_OFFSET + i, 0, 0, 0);
      }
      leds.show();
      delay(DELAY);   
    }
  }else if(op_mode == OP_RANDOM){
    for(int i = 0; i < LED_COUNT; i++){
      leds.setPixelColor(CENTER_OFFSET + i, random(0, 255), random(0, 255), random(0, 255));
    }
    leds.show();
    delay(DELAY);
  }else if(op_mode == OP_RAINFLOW){
    for(int i = 0; i < LED_COUNT; i++){
      float prog = i / (float)LED_COUNT;
      struct rgb_value r = rainbow(counter + prog);
      leds.setPixelColor(CENTER_OFFSET + i, r.r, r.g, r.b);
    }
    counter += 0.0025f;
    leds.show();
    delay(DELAY);
  }else if(op_mode == OP_FILL){
    if(intensity1[1] != 0 && intensity1[2] != 0){
      for(int i = 0; i < LED_COUNT; i++){
        intensity1[i] = 0;
        leds.setPixelColor(CENTER_OFFSET + i, 0, 0, 0);
      }
    }
    if(intensity1[current + 1] != 0 || current + 1 > LED_COUNT / 2){
      current = 0;
    }else{
      leds.setPixelColor(CENTER_OFFSET + current, 0, 0, 0);
      leds.setPixelColor(CENTER_OFFSET + LED_COUNT - current, 0, 0, 0);
      intensity1[current] = 0;
      current++;
      intensity1[current] = 255;
      leds.setPixelColor(CENTER_OFFSET + current, 255, 0, 0);
      leds.setPixelColor(CENTER_OFFSET + LED_COUNT - current, 0, 255, 0);
    }
    leds.show();
    delay(DELAY);
  }
  circle += 0.01f;
}

