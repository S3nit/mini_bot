#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Master Eye Config ---
float baseW = 26, baseH = 38;
int eyeSpacing = 28;

// --- Physics & LERP Variables ---
float curW = 0, tarW = 26; 
float curH = 0, tarH = 38;
float curY = 32, tarY = 32;
float curX = 0,  tarX = 0;
float curP = 0,  tarP = 5; 

float leftEyeHScale = 1.0; 
float rightEyeHScale = 1.0; // Added for asymmetrical moods
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
  tarW = baseW; tarH = baseH; tarP = 5;
}

void updateDisplay() {
  display.clearDisplay();
  int centerX = 64;

  // LERP for smooth motion (Keeping your exact 0.12 speed)
  curW += (tarW - curW) * 0.12;
  curH += (tarH - curH) * 0.12;
  curY += (tarY - curY) * 0.12;
  curX += (tarX - curX) * 0.12;
  curP += (tarP - curP) * 0.12;

  float pX = curX * 0.45;
  float pY = (curY - 32) * 0.45;

  // 1. DRAW BASE WHITE EYES
  float lH = curH * leftEyeHScale;
  float rH = curH * rightEyeHScale;

  // Left Eye
  if (mood == 6 && leftEyeHScale < 0.3) { 
    display.drawFastHLine(centerX - eyeSpacing - (curW/2) + curX, curY, curW, WHITE);
    display.drawFastHLine(centerX - eyeSpacing - (curW/2) + curX + 2, curY - 1, curW - 4, WHITE);
  } else {
    display.fillRoundRect(centerX - eyeSpacing - (curW/2) + curX, curY - (lH/2), curW, lH, curW/2, WHITE);
  }

  // Right Eye
  display.fillRoundRect(centerX + eyeSpacing - (curW/2) + curX, curY - (rH/2), curW, rH, curW/2, WHITE);

  // 2. DRAW BLACK PUPILS
  if(curP > 0.5 && curH > 10) {
    // Left Pupil
    if(!(mood == 6 && leftEyeHScale < 0.3) && lH > 10) { 
      display.fillCircle(centerX - eyeSpacing + curX + pX, curY + pY, curP, BLACK);
      display.fillCircle(centerX - eyeSpacing + curX + pX + 1, curY + pY - 1, 1, WHITE);
    }
    // Right Pupil
    if(rH > 10) {
      display.fillCircle(centerX + eyeSpacing + curX + pX, curY + pY, curP, BLACK);
      display.fillCircle(centerX + eyeSpacing + curX + pX + 1, curY + pY - 1, 1, WHITE);
    }
  }

  // 3. DRAW EMOTION OVERLAYS
  if (mood == 1) { // PROUD Overlay
     display.fillRect(0, curY + (curH * 0.25), 128, 30, BLACK);
     int mouthW = 12 + (sin(millis() * 0.003) * 2);
     display.drawFastHLine(centerX - (mouthW/2), curY + 20, mouthW, WHITE);
     display.drawPixel(centerX - (mouthW/2), curY + 19, WHITE);
     display.drawPixel(centerX + (mouthW/2), curY + 19, WHITE);
  }
  
  if (mood == 4) { // SINGING
    float pulse = abs(sin(millis()*0.015));
    display.fillCircle(centerX, curY + 22, 3 + (pulse*5), WHITE);
    display.fillCircle(centerX, curY + 22, 1 + (pulse*3), BLACK);
  }

  if (mood == 10) { // ANGRY Brows
    display.fillTriangle(centerX-40, curY-22, centerX-10, curY-18, centerX-10, curY-30, BLACK);
    display.fillTriangle(centerX+40, curY-22, centerX+10, curY-18, centerX+10, curY-30, BLACK);
  }

  display.display();
}

void loop() {
  unsigned long now = millis();
  leftEyeHScale = 1.0;
  rightEyeHScale = 1.0;

  long currentDuration = (mood == 0) ? 20000 : 10000;

  if (now - lastMoodSwitch > currentDuration) {
    mood = (mood + 1) % 14; // Updated to 14 moods
    lastMoodSwitch = now;
    gazeStep = 0; 
    
    float oldH = tarH; tarH = 1; 
    for(int i=0; i<6; i++) { updateDisplay(); delay(15); }
    tarH = oldH;
  }

  switch (mood) {
    case 0: // NORMAL (Keeping your exact gaze logic)
      tarW = baseW + cos(now * 0.002) * 1.5;
      tarH = baseH + sin(now * 0.002) * 1.5;
      tarP = 5;
      if (now - lastGazeChange > 2800) { gazeStep = (gazeStep + 1) % 7; lastGazeChange = now; }
      if(gazeStep == 0) { tarX = 0;   tarY = 32; }
      if(gazeStep == 1) { tarX = 0;   tarY = 20; } 
      if(gazeStep == 2) { tarX = 0;   tarY = 44; } 
      if(gazeStep == 3) { tarX = -18; tarY = 32; } 
      if(gazeStep == 4) { tarX = 18;  tarY = 32; } 
      if(gazeStep == 5) { tarX = -15; tarY = 22; } 
      if(gazeStep == 6) { tarX = 15;  tarY = 22; } 
      break;

    // --- YOUR ORIGINAL MOODS (Untouched) ---
    case 1: tarW = 32; tarH = 24; tarY = 24; tarP = 4; tarX = 0; break; // PROUD
    case 2: tarW = 30; tarH = 16; tarY = 42; tarP = 3; tarX = 0; break; // SAD
    case 3: tarW = 32; tarH = 10; tarY = 32; tarP = 3; tarX = 0; break; // CHILLIN
    case 4: tarY = 25 + sin(now * 0.015) * 4; tarW = 24; tarH = 24; tarP = 0; break; // SINGING
    case 5: { // DANCING
      float dW = sin(now * 0.012);
      tarY = 32 + (dW * 12); tarX = cos(now * 0.006) * 18;
      tarW = baseW - (dW * 6); tarH = baseH + (dW * 8); tarP = 4;
      break; }
    case 6: // WINK
      tarW = baseW; tarH = baseH; tarY = 30; tarX = 10; tarP = 5;
      if (sin(now * 0.01) > 0.0) leftEyeHScale = 0.1; else leftEyeHScale = 1.0;
      break;
    case 7: // PETTING
      tarW = 34; tarH = 22; tarY = 28 + (sin(now * 0.02) * 2); tarX = 0; tarP = 8;
      break;
    case 8: tarW = 10; tarH = 10; tarY = 32; tarP = 0; tarX = 0; break; // QUIET

    // --- NEW ADDITIONAL MOODS ---
    case 9: // THINKER (Orbiting)
      tarW = 16; tarH = 16; tarP = 0;
      tarX = cos(now * 0.003) * 15;
      tarY = 32 + sin(now * 0.003) * 8;
      break;

    case 10: // ANGRY
      tarW = 28; tarH = 30; tarY = 36; tarP = 3;
      tarX = sin(now * 0.06) * 2; // Frustrated jitter
      break;

    case 11: // DIZZY
      tarW = 22; tarH = 22; tarP = 4;
      leftEyeHScale = 0.6 + abs(sin(now * 0.012));
      rightEyeHScale = 0.6 + abs(cos(now * 0.012));
      tarX = sin(now * 0.005) * 12;
      break;

    case 12: // SURPRISED
      tarW = 18; tarH = 46; tarY = 32; tarP = 2;
      break;

    case 13: // SLEEPY
      tarW = 28; tarH = 4; tarY = 34; tarP = 0;
      if(random(100) > 96) tarH = 14; // Struggle to wake up
      break;
  }

  // Random Blink (Disabled for continuous expression moods)
  if (random(1000) > 993 && mood != 3 && mood != 6 && mood != 7 && mood != 12 && mood != 13) {
    float oldH = tarH; tarH = 1; 
    updateDisplay(); delay(70); 
    tarH = oldH;
  }

  updateDisplay();
  delay(10); 
}
