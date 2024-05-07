#ifndef LED_H
#define LED_H

#include "Adafruit_NeoPixel.h"

#define LED_PIN   D2
#define NUMPIXELS 10

/* ------------------------------ Color macros ------------------------------ */
#define PINK   255, 0, 127
#define BLUE   0, 0, 255
#define RED    255, 0, 0
#define BLACK  0, 0, 0
#define YELLOW 255, 255, 168
/* -------------------------------------------------------------------------- */

extern Adafruit_NeoPixel pixels;

extern void neoPixelSetup();
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