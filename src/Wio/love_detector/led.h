#ifndef LED_H
#define LED_H

#include "Adafruit_NeoPixel.h"

#define LED_PIN D2
#define NUMPIXELS    10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void neoPixelSetup(){
  //Neopixels Setup
  randomSeed(analogRead(LED_PIN));
  pixels.setBrightness(255);
  pixels.begin(); // This initializes the NeoPixel library.

    // Set all pixels to "off" (black)
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
     pixels.show();

}

extern void blinkSequence(uint32_t color);
extern void lightUpSequence(uint32_t color);
extern void fadeInSequence();
extern void upAndDown();
extern void slowFillUp(uint32_t color);
extern void noMatchMode(uint32_t color);
extern void normalMode(uint32_t color);
extern void crazyMode();
extern void light(int level);

#endif