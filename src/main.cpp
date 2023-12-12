#include "TKPoint.h"
#include "WiiNunchuck.h"
#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SharpMem.h>
#include <Arduino.h>

// keep track of time
unsigned long lastFrame = 0;

// SHARP display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, 400, 240, 8000000);
// can go up to 8000000

// WiiNunchuck
WiiNunchuck chuck;

// POINTS
TKPoint p1;
TKPoint p2;
TKPoint p3;

TKPoint v1;
TKPoint v2;
TKPoint v3;

// RGB
Adafruit_NeoPixel pixels(1, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

void setupDisplay() {
  // SPI.begin(SCK, -1, MOSI, CS);
  // MOSI defaults to 11, SCK to 12
  SPI.begin(PIN_LCD_CLK, -1, PIN_LCD_DI, PIN_LCD_CS);
  display.begin();
  display.setRotation(0);
  display.clearDisplay();
}

void setup() {
  Serial.begin(57600);
  delay(1000);
  Serial.println("Hello World");
  delay(100);

  setupDisplay();

  pixels.begin();
  // fade in blue
  for (int i = 0; i < 255; i++) {
    pixels.setPixelColor(0, pixels.Color(0, 0, i));
    pixels.show();
    delay(5);
  }
  // blue to red
  for (int i = 0; i < 255; i++) {
    pixels.setPixelColor(0, pixels.Color(i, 0, 255 - i));
    pixels.show();
    delay(5);
  }
  // fade out red
  for (int i = 0; i < 255; i++) {
    pixels.setPixelColor(0, pixels.Color(255 - i, 0, 0));
    pixels.show();
    delay(5);
  }
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();

  // setup chuck
  chuck.begin(PIN_SDA, PIN_SCL);

  // setup the points somewhere on the screen
  p1.set(random(400), random(240));
  p2.set(random(400), random(240));
  p3.set(random(400), random(240));
  v1.set(random(-100, 100), random(-100, 100));
  v2.set(random(-100, 100), random(-100, 100));
  v3.set(random(-100, 100), random(-100, 100));
  v1.normalize();
  v2.normalize();
  v3.normalize();

  // write something!
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0, 5);
  display.println(" Helloooo world!");
  display.println(" Compiled: " __DATE__ " " __TIME__);
  display.refresh();
  Serial.println("Display setup done!");
  delay(1000);
}

void movePoints() {
  p1 = p1 + v1;
  p2 = p2 + v2;
  p3 = p3 + v3;
  if (p1.x < 0 || p1.x > 400) {
    v1.x *= -1;
  }
  if (p1.y < 0 || p1.y > 240) {
    v1.y *= -1;
  }
  if (p2.x < 0 || p2.x > 400) {
    v2.x *= -1;
  }
  if (p2.y < 0 || p2.y > 240) {
    v2.y *= -1;
  }
  if (p3.x < 0 || p3.x > 400) {
    v3.x *= -1;
  }
  if (p3.y < 0 || p3.y > 240) {
    v3.y *= -1;
  }
}

void loop() {
  unsigned long now = millis();

  if (now > lastFrame + 1000 / 30) { // 60fps
    float frameTime = now - lastFrame;
    int FPS = 0;
    if (frameTime > 0) {
      FPS = 1000 / frameTime;
    }
    // reset the timer
    lastFrame = now;

    movePoints();

    // clear the display
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(5, 5);
    display.println(FPS);

    // display.fillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 4);
    display.drawLine(p1.x, p1.y, p2.x, p2.y, BLACK);
    display.drawLine(p1.x, p1.y, p3.x, p3.y, BLACK);
    display.drawLine(p3.x, p3.y, p2.x, p2.y, BLACK);

    // draw chuck location
    chuck.update();
    chuck.printRaw();
    TKPoint cp;
    cp.set(chuck.joyX, chuck.joyY);
    TKPoint cc;
    cc.set(400 / 2, 240 / 2);
    cp = cp + cc;
    display.drawLine(cp.x - 10, cp.y, cp.x + 10, cp.y, BLACK);
    display.drawLine(cp.x, cp.y - 10, cp.x, cp.y + 10, BLACK);

    // if(chuck.c_button){
    //   display.fillCircle(20, 20, 5, BLACK);
    // }
    // if(chuck.z_button){
    //   display.fillCircle(40, 20, 5, BLACK);
    // }
    for (int i = 0; i < 8; i++) {
      if ((chuck.b >> i) & 0x01) {
        display.fillCircle(20 + i * 20, 20, 5, BLACK);
      } else {
        display.drawCircle(20 + i * 20, 20, 5, BLACK);
      }
    }

    display.fillRect(400 / 4, 240 / 2, 4, chuck.aX - 128, GRAY);
    display.fillRect(400 / 2, 240 / 2, 4, chuck.aY - 128, GRAY);
    display.fillRect(3 * 400 / 4, 240 / 2, 4, chuck.aZ - 128, GRAY);

    // send out to display
    display.refresh();
  }
}
