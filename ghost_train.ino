#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <Servo.h>

#define TFT_DC 7
#define TFT_CS 10

// Hardware SPI on Feather or other boards
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);


int hour = 11;
int minute = 55;
unsigned long lastUpdate = 0;
bool showClock = true;

int pos = 0;
Servo servo;

void setup() {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(GC9A01A_BLACK);
  servo.attach(9);
  servo.write(pos);
}

void loop() {
  unsigned long now = millis();

  // jede 60 Sekunden +1 Minute simulieren
  if (now - lastUpdate > 60000) {
    minute++;
    if (minute >= 60) {
      minute = 0;
      hour = (hour + 1) % 12;
    }
    lastUpdate = now;
  }

  // alle 10 Sekunden Wechsel zwischen Uhr und Geist
  if ((now / 10000) % 2 == 0) {
    if (!showClock) {
      drawClock(hour, minute);
      showClock = true;
      moveMouth();
    }
  } else {
    if (showClock) {
      drawGhost();
      showClock = false;
      servo.write(pos);
    }
  }
}

void moveMouth() {
  for (int j = 0; j < 5; j++) {
    servo.write(pos);
    delay(50);
    servo.write(pos + 10);
    delay(50);
  }
}

// Analoge Uhr
void drawClock(int h, int m) {
  drawClockFace();          // Hintergrund neu
  drawHands(hour, minute);  // Zeiger neu
}

// Einfacher Geist
void drawGhost() {
  tft.fillScreen(GC9A01A_BLACK);

  int cx = 120, cy = 100;

  // Körper
  tft.fillCircle(cx, cy, 60, GC9A01A_WHITE);
  tft.fillRect(cx - 60, cy, 120, 80, GC9A01A_WHITE);

  // Augen
  tft.fillCircle(cx - 20, cy - 10, 10, GC9A01A_BLACK);
  tft.fillCircle(cx + 20, cy - 10, 10, GC9A01A_BLACK);

  // Mund
  tft.fillCircle(cx, cy + 20, 15, GC9A01A_BLACK);

  // Schrift
  tft.setCursor(70, 200);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);
  tft.print("Buuuuuuh!");
}

void drawClockFace() {
  int cx = 120, cy = 120, r = 110;

  tft.fillScreen(GC9A01A_BLACK);

  // Außenring
  tft.drawCircle(cx, cy, r, GC9A01A_WHITE);
  tft.drawCircle(cx, cy, r - 1, GC9A01A_WHITE);

  // Römische Ziffern
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);
  const char* numerals[12] = { "XII", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI" };
  for (int i = 0; i < 12; i++) {
    float angle = (i * 30 - 60) * PI / 180;  // -60 = Start oben
    int x = cx + cos(angle) * (r - 25);
    int y = cy + sin(angle) * (r - 25);
    tft.setCursor(x - 10, y - 8);
    tft.print(numerals[i]);
  }
}

void drawHands(int h, int m) {
  int cx = 120, cy = 120;

  // Stundenzeiger
  float hAngle = ((h % 12) + m / 60.0) * 30 * PI / 180;
  int hx = cx + cos(hAngle - PI / 2) * 50;
  int hy = cy + sin(hAngle - PI / 2) * 50;
  tft.drawLine(cx, cy, hx, hy, GC9A01A_WHITE);
  tft.drawLine(cx + 1, cy, hx + 1, hy, GC9A01A_WHITE);

  // Minutenzeiger
  float mAngle = m * 6 * PI / 180;
  int mx = cx + cos(mAngle - PI / 2) * 80;
  int my = cy + sin(mAngle - PI / 2) * 80;
  tft.drawLine(cx, cy, mx, my, GC9A01A_WHITE);

  // Mittelpunkt
  tft.fillCircle(cx, cy, 3, GC9A01A_WHITE);
}