#ifndef LED_H
#define LED_H                                             // header guard

#include "Adafruit_NeoPixel.h"                            // library for led lights

#define LED_PIN   D2                                      // define the digital pin for the led lights
#define NUMPIXELS 10                                      // define the number of pixels for the led lights (number of "lights")

/* ------------------------------ Color macros ------------------------------ */

#define PINK   255, 0, 127                                // color codes in RGB scheme
#define BLUE   0, 0, 255
#define RED    255, 0, 0
#define BLACK  0, 0, 0
#define YELLOW 255, 255, 168

/* --------------------- LED functions for animations ----------------------- */

extern Adafruit_NeoPixel pixels;

// function that sets up lights to be ready for displaying colors
extern void neoPixelSetup();

// function that triggers lights to blink on/off a number of times
extern void blinkSequence(uint32_t color);

// function to turn the lights on one by one
extern void lightUpSequence(uint32_t color);

// function to turn the lights on one by one with a slower/delayed effect
extern void slowLightUpSequence(uint32_t color);

// function to create a "fading out" effect for all lights
extern void fadeInSequence();

// function to turn the lights on in different colors in different "directions"
extern void biDirectionalSequence();

// function for a light animation when no real connection is detected
extern void levelOneMode(uint32_t color);

// function for a light animation when a possible connection is detected
extern void levelTwoMode(uint32_t color);

// function for a light animation when a strong connection is detected
extern void levelThreeMode(uint32_t color);

// function to display appropriate animation based on date result
extern void startLightAnimation(int level);

/* ---------------------------------------------------------------------------- */

#endif                                                    // end header guard