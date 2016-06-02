#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#define DI_PIN 4
#define SENSOR_PIN 0
#define MAX_LED_COUNT 150
#define PI 3.14159265
#define PI_2 PI / 2
#define SLAVE_ADDRESS 0x05

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

char message[64];
bool messageReady = false;

struct rgb_value color1 = rgb(255, 0, 0);
struct rgb_value color2 = rgb(0, 255, 0);
struct rgb_value color3 = rgb(0, 0, 255);
float speedMod = 1.0f;

void changeOpMode(int op){
  for(int i = 0; i < op_modes[op_mode].ledCount; i++){
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();
  
  op_mode = op;

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
  
  changeOpMode(OP_FILL);

  Serial.begin(9600);

  for(int i = 0; i < 64; i++){
    message[i] = '\0';
  }

  /*char *msg = "4.#FFFF00.#00FFFF.#FF00FF.7530.";
  strncpy(message, msg, strlen(msg));

  messageReady = true;*/
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

void receiveData(int byteCount){
  int i = 0;

  if(byteCount > 0){
    while(Wire.available() > 0){
      char c = Wire.read();
      message[i++] = c;
    }

    Serial.println(i);

    if(i > 0){
      messageReady = true;
    }
  }
}

void sendData(){
  // Any call to delay(X) causes an IOError on the python side of things
  // I think there may be an issue with taking too much time to compute inside this callback. Or transferring control using the delay() method. 
  // Our workaround sets these flags, does the computation inside loop(), and continually sends the errorRetry message when the encryption hasn't yet been computed. 
  
  char success[] = {'S', 'u', 'c', 'c', 'e', 's', 's'};
  Wire.write(success, 7);
}

void loop() {
  int brightness = analogRead(0);
  leds.setBrightness(brightness);

  if(messageReady){
    messageReady = false;
    Serial.print("Message: ");
    int len = 0;
    for(len = 0; len < 32; len++){
      if(len != 0 && message[len] == '\0'){
        break;
      }
      Serial.print(message[len]);
    }
    Serial.print(" - Len ");
    Serial.println(len);
    
    int scriptID = -2;
    int speed = -3;

    bool scriptIDSet = false;
    bool color1Set = false;
    bool color2Set = false;
    bool color3Set = false;
    bool speedSet = false;

    char *copy = (char *)malloc(sizeof(char) * len);
    strncpy(copy, &message[1], len);

    for(int i = 0; i < 64; i++){
      message[i] = '\0';
    }

    char *token = strtok(copy, ".");
    Serial.println(token);
    while(token != NULL){
      if(strlen(token) > 0){
        Serial.print("Token: ");
        Serial.println(token);
        if(token[0] == '#'){
          if(strlen(token) == 7){
            // break out components into strings with null terminators, so strtol doesn't read junk data
            char rhex[3] = {token[1], token[2], '\0'};
            char ghex[3] = {token[3], token[4], '\0'};
            char bhex[3] = {token[5], token[6], '\0'};
            int r = strtol(&rhex[0], NULL, 16); // 16 bit HEX representation
            int g = strtol(&ghex[0], NULL, 16);
            int b = strtol(&bhex[0], NULL, 16);
            if(!color1Set){
              color1 = rgb(r, g, b);
              color1Set = true;
            }else if(!color2Set){
              color2 = rgb(r, g, b);
              color2Set = true;
            }else if(!color3Set){
              color3 = rgb(r, g, b);
              color3Set = true;
            }
          }else{
            Serial.println("Color token length not 7");
          }
        }else{
          int num = atoi(token);
          if(!scriptIDSet){
            scriptID = num;
            scriptIDSet = true;
          }else if(!speedSet){
            speed = num;
            speedSet = true;
          }
        }
      }
      token = strtok(NULL, ".");
    }

    free(copy);

    Serial.println("Message processing complete:");
    
    if(color1Set){
      Serial.print("Color 1:");
      Serial.print(color1.r); Serial.print(" ");
      Serial.print(color1.g); Serial.print(" ");
      Serial.print(color1.b); Serial.print(" ");
      Serial.println(" ");
    }
    if(color2Set){
      Serial.print("Color 2:");
      Serial.print(color2.r); Serial.print(" ");
      Serial.print(color2.g); Serial.print(" ");
      Serial.print(color2.b); Serial.print(" ");
      Serial.println(" ");
    }
    if(color3Set){
      Serial.print("Color 3:");
      Serial.print(color3.r); Serial.print(" ");
      Serial.print(color3.g); Serial.print(" ");
      Serial.print(color3.b); Serial.print(" ");
      Serial.println(" ");
    }
    if(speedSet){
      Serial.print("Speed: "); Serial.println(speed);
      speedMod = speed / 50.0f;
    }
    if(scriptIDSet){
      Serial.print("ScriptID: "); Serial.println(scriptID);
      changeOpMode(scriptID - 1); // from 1-indexed to 0-indexed
    }
  }

  if(Serial.available() > 0){
    int mode = Serial.read();
    mode = mode - '0';
    
    changeOpMode(mode);
    messageReady = true;
  }

  int LED_COUNT = op_modes[op_mode].ledCount;   
  int DELAY = speedMod * op_modes[op_mode].msDelay;
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
          leds.setPixelColor(CENTER_OFFSET + i, color1.r, color1.g, color1.b);
        else if(i % 6 == (f + 1) % 6)
          leds.setPixelColor(CENTER_OFFSET + i, color2.r, color2.g, color2.b);
        else if(i % 6 == (f + 2) % 6)
          leds.setPixelColor(CENTER_OFFSET + i, color3.r, color3.g, color3.b);
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
      leds.setPixelColor(CENTER_OFFSET + current, color1.r, color1.g, color1.b);
      leds.setPixelColor(CENTER_OFFSET + LED_COUNT - current, color2.r, color2.g, color2.b);
    }
    leds.show();
    delay(DELAY);
  }
  circle += 0.01f;
}

