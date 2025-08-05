#include <Deneyap_KumandaKolu.h>
#include <Deneyap_5x7LedEkran.h>

Joystick joystick;
DotMatrix ledMatrix;

#define ROWS 7
#define COLS 5
#define MAX_LENGTH (ROWS * COLS)

struct Point {
  int x;
  int y;
};

Point snake[MAX_LENGTH];
int snakeLength = 3;  

byte bitmap[ROWS * COLS];

Point food = {4, 6};  

bool gameOver = false;

unsigned long lastDrawTime = 0;
const unsigned long drawInterval = 20; // ms cinsinden, 150 ms aralıkla güncelle

void setup() {
  Serial.begin(115200);

  if (!joystick.begin(0x1B)) {
    Serial.println("Joystick başlatılamadı!");
    while (1);
  }

  if (!ledMatrix.begin(0x0A)) {
    Serial.println("LED Matris başlatılamadı!");
    while (1);
  }

  Serial.println("Modüller başlatıldı.");

  snake[0] = {1, 3};
  snake[1] = {0, 3};
  snake[2] = {0, 2};

  drawGame();
  lastDrawTime = millis();
}

void loop() {
  int dx = 0;
  int dy = 0;

  uint16_t xVal = joystick.xRead();
  uint16_t yVal = joystick.yRead();

  if (xVal > 520) dx = 1;
  else if (xVal < 470) dx = -1;

  if (yVal > 520) dy = -1;
  else if (yVal < 470) dy = 1;

  if (gameOver) {
    if (dx != 0 || dy != 0) {
      resetGame();
      drawGame();
      lastDrawTime = millis();
    }
    return ; // Oyun bittiğinde joystick hareket bekle
  }

  if (dx != 0 || dy != 0) {
    int newX = snake[0].x + dx;
    int newY = snake[0].y + dy;

    if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && !isSnake(newX, newY)) {
      moveSnake(newX, newY);
      drawGame();
      lastDrawTime = millis();

      if (newX == food.x && newY == food.y) {
        Serial.println("Tebrikler, çıkış noktasına ulaştınız! Oyun bitti.");
        gameOver = true;
      }
    } else {
      Serial.println("Çarpma! Oyun bitti.");
      gameOver = true;
    }
  }

  if (millis() - lastDrawTime > drawInterval) {
    drawGame();
    lastDrawTime = millis();
  }
}

bool isSnake(int x, int y) {
  for (int i = 0; i < snakeLength; i++) {
    if (snake[i].x == x && snake[i].y == y) return true;
  }
  return false;
}

void moveSnake(int newX, int newY) {
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0] = {newX, newY};
}

void drawGame() {
  clearBitmap();
  setPixel(food.x, food.y, true);

  for (int i = 0; i < snakeLength; i++) {
    setPixel(snake[i].x, snake[i].y, true);
  }

  ledMatrix.drawLedMatrix(bitmap);
}

void clearBitmap() {
  for (int i = 0; i < ROWS * COLS; i++) {
    bitmap[i] = 0;
  }
}

void setPixel(int x, int y, bool on) {
  if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
  int index = y * COLS + x;
  bitmap[index] = on ? 1 : 0;
}

void resetGame() {
  snakeLength = 3;
  snake[0] = {1, 3};
  snake[1] = {0, 3};
  snake[2] = {0, 2};
  gameOver = false;
  drawGame();
}
