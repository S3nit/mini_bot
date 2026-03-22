#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float baseW = 26;        
float baseH = 38;        
int eyeSpacing = 28;     
int eyeY = 32;           

float curW = 0, tarW = 0; 
float curH = 0, tarH = 0;
float curY = 32, tarY = 32;
float curX = 0,  tarX = 0;
float curP = 0,  tarP = 6; 

float leftEyeHScale = 1.0; 
int mood = 0; 
unsigned long lastMoodSwitch = 0;

int gazeStep = 0;
unsigned long lastGazeChange = 0;

void setup() {
  Wire.begin(8, 9); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display(); 
  delay(1000); 
  tarW = baseW; tarH = baseH; tarP = 6;
}

void updateDisplay() {
  display.clearDisplay();
  int centerX = 64;

  curW += (tarW - curW) * 0.12;
  curH += (tarH - curH) * 0.12;
  curY += (tarY - curY) * 0.12;
  curX += (tarX - curX) * 0.12;
  curP += (tarP - curP) * 0.12;

  float pX = curX * 0.45;
  float pY = (curY - 32) * 0.45;

  float lH = curH * leftEyeHScale;
  display.fillRoundRect(centerX - eyeSpacing - (curW/2) + curX, curY - (lH/2), curW, lH, curW/2, WHITE);
  
  display.fillRoundRect(centerX + eyeSpacing - (curW/2) + curX, curY - (curH/2), curW, curH, curW/2, WHITE);

  if(curP > 0.5 && curH > 8) {
    if(lH > 8) {
      display.fillCircle(centerX - eyeSpacing + curX + pX, curY + pY, curP, BLACK);
      display.fillCircle(centerX - eyeSpacing + curX + pX + 1, curY + pY - 1, 1, WHITE);
    }
    display.fillCircle(centerX + eyeSpacing + curX + pX, curY + pY, curP, BLACK);
    display.fillCircle(centerX + eyeSpacing + curX + pX + 1, curY + pY - 1, 1, WHITE);
  }

  if (mood == 1) { 
     display.fillCircle(centerX - eyeSpacing + curX, curY + (curH*0.45), curW, BLACK);
     display.fillCircle(centerX + eyeSpacing + curX, curY + (curH*0.45), curW, BLACK);
  }
  if (mood == 4) { 
    float pulse = abs(sin(millis()*0.015));
    display.fillCircle(centerX, curY + 22, 3 + (pulse*5), WHITE);
    display.fillCircle(centerX, curY + 22, 1 + (pulse*3), BLACK);
  }

  display.display();
}

void loop() {
  unsigned long now = millis();
  leftEyeHScale = 1.0;

  long currentDuration = (mood == 0) ? 20000 : 10000;

  if (now - lastMoodSwitch > currentDuration) {
    mood = (mood + 1) % 8; 
    lastMoodSwitch = now;
    gazeStep = 0; 
    
    float oldH = tarH; tarH = 1; 
    for(int i=0; i<6; i++) { updateDisplay(); delay(15); }
    tarH = oldH;
  }

  switch (mood) {
    case 0: 
      tarW = baseW + cos(now * 0.002) * 1.5;
      tarH = baseH + sin(now * 0.002) * 1.5;
      tarP = 5;
      if (now - lastGazeChange > 2800) {
        gazeStep = (gazeStep + 1) % 7;
        lastGazeChange = now;
      }
      if(gazeStep == 0) { tarX = 0;   tarY = 32; }
      if(gazeStep == 1) { tarX = 0;   tarY = 20; } 
      if(gazeStep == 2) { tarX = 0;   tarY = 44; } 
      if(gazeStep == 3) { tarX = -18; tarY = 32; } 
      if(gazeStep == 4) { tarX = 18;  tarY = 32; } 
      if(gazeStep == 5) { tarX = -15; tarY = 22; } 
      if(gazeStep == 6) { tarX = 15;  tarY = 22; } 
      break;

    case 1: tarW = 34; tarH = 34; tarY = 28; tarP = 0; break;
    case 2: tarW = 30; tarH = 16; tarY = 42; tarP = 3; tarX = 0; break;
    case 3: tarW = 32; tarH = 10; tarY = 32; tarP = 3; tarX = 0; break;
    case 4: tarY = 25 + sin(now * 0.015) * 4; tarW = 24; tarH = 24; tarP = 0; break;
    case 5: { 
      float dW = sin(now * 0.012);
      tarY = 32 + (dW * 12); tarX = cos(now * 0.006) * 18;
      tarW = baseW - (dW * 6); tarH = baseH + (dW * 8); tarP = 4;
      break; }
    case 6: 
      tarW = baseW; tarH = baseH; tarY = 32; tarX = 6; tarP = 5;
      leftEyeHScale = 0.1 + abs(sin(now * 0.006)) * 0.9; break;
    case 7: tarW = 10; tarH = 10; tarY = 32; tarP = 0; tarX = 0; break;
  }

  if (random(1000) > 993 && mood != 1 && mood != 3) {
    float oldH = tarH; tarH = 1; 
    updateDisplay(); delay(70); 
    tarH = oldH;
  }

  updateDisplay();
  delay(10); 
}
