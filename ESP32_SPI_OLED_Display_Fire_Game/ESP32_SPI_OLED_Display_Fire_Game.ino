#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  4

#define JOY_X_PIN   32
#define JOY_Y_PIN   33
#define JOY_SW_PIN  12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int playerX = SCREEN_WIDTH / 2;
const int playerY = SCREEN_HEIGHT - 8;
bool bulletActive = false;
int bulletX = playerX, bulletY = playerY - 8;

int targetX = random(10, SCREEN_WIDTH - 10);
int targetY = 10;
const int targetRadius = 6;

int score = 0;

bool lastFire = false;

void setup() {
  Serial.begin(115200);
  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println("GUN FIRE!");
  display.display();
  delay(1200);
}

void loop() {
  gunFireGame();
}

void gunFireGame() {

  int joyX = analogRead(JOY_X_PIN);
  int mappedX = map(joyX, 0, 4095, 10, SCREEN_WIDTH - 10);
  playerX += constrain(mappedX - playerX, -4, 4);

  bool firePressed = digitalRead(JOY_SW_PIN) == LOW;
  if (firePressed && !lastFire && !bulletActive) {
    bulletActive = true;
    bulletX = playerX;
    bulletY = playerY - 8;
  }
  lastFire = firePressed;

  if (bulletActive) {
    bulletY -= 6;
    if (bulletY < 0) {
      bulletActive = false;
    }
    
    int dx = bulletX - targetX;
    int dy = bulletY - targetY;
    if ((abs(dx) <= targetRadius) && (abs(dy) <= targetRadius)) {
      bulletActive = false;
      score++;
      targetX = random(10, SCREEN_WIDTH - 10);
      targetY = random(8, SCREEN_HEIGHT / 2);
    }
  }

  display.clearDisplay();
  
  display.fillTriangle(playerX, playerY, playerX-8, playerY+8, playerX+8, playerY+8, SSD1306_WHITE);

  if (bulletActive)
    display.fillRect(bulletX-1, bulletY-5, 3, 7, SSD1306_WHITE);

  display.drawCircle(targetX, targetY, targetRadius, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print("Score: ");
  display.print(score);

  display.display();
  delay(36);
}
