#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- New "Large" Eye Settings ---
int eyeSize = 38;       // Much taller (was 24)
int eyeWidth = 28;      // Much wider (was 18)
int radius = 14;        // Smoother corners for the larger size
int eyeSpacing = 32;    // Closer together for that "funko pop" look
int eyeY = 32;          // Centered vertically

// --- Animation Variables ---
float xOffset = 0;      
float targetX = 0;      
unsigned long lastAction = 0;

void setup() {
  Wire.begin(8, 9); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void drawBigEyes(float xShift, float yScale) {
  display.clearDisplay();
  
  int centerX = SCREEN_WIDTH / 2;
  int currentH = eyeSize * yScale;
  
  // Blink Logic (yScale goes to 0)
  if (yScale < 0.15) {
    int blinkW = eyeWidth + 4; // Make the blink line slightly wider than the eye
    display.fillRoundRect(centerX - eyeSpacing - (blinkW/2) + xShift, eyeY, blinkW, 4, 2, WHITE);
    display.fillRoundRect(centerX + eyeSpacing - (blinkW/2) + xShift, eyeY, blinkW, 4, 2, WHITE);
  } else {
    // 1. Draw Left Eye Base
    display.fillRoundRect(centerX - eyeSpacing - (eyeWidth/2) + xShift, eyeY - (currentH/2), eyeWidth, currentH, radius, WHITE);
    // 2. Draw Right Eye Base
    display.fillRoundRect(centerX + eyeSpacing - (eyeWidth/2) + xShift, eyeY - (currentH/2), eyeWidth, currentH, radius, WHITE);
    
    // 3. Add Large Pupils (The "Soul" of the robot)
    // Pupils stay centered vertically but move horizontally with the gaze
    float pupilX = xShift * 0.5; 
    display.fillCircle(centerX - eyeSpacing + xShift + pupilX, eyeY, 5, BLACK);
    display.fillCircle(centerX + eyeSpacing + xShift + pupilX, eyeY, 5, BLACK);
    
    // 4. Add a tiny white "glint" inside the pupil for extra sparkle
    display.fillCircle(centerX - eyeSpacing + xShift + pupilX + 1, eyeY - 1, 1, WHITE);
    display.fillCircle(centerX + eyeSpacing + xShift + pupilX + 1, eyeY - 1, 1, WHITE);
  }
  
  display.display();
}

void loop() {
  unsigned long now = millis();

  // Smooth movement Interpolation
  xOffset += (targetX - xOffset) * 0.15;

  // Decide to act
  if (now - lastAction > random(3000, 7000)) {
    // Smooth Animated Blink
    for (float s = 1.0; s >= 0; s -= 0.25) { drawBigEyes(xOffset, s); delay(10); } 
    for (float s = 0; s <= 1.0; s += 0.25) { drawBigEyes(xOffset, s); delay(10); } 
    
    // Pick a random "look" direction
    targetX = random(-15, 16); 
    lastAction = now;
  }

  drawBigEyes(xOffset, 1.0);
  delay(20); 
}
