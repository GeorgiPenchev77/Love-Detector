#include "led.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);


void neoPixelSetup() {
  randomSeed(analogRead(LED_PIN));
  pixels.setBrightness(255);
  pixels.begin();                                        // initialization for the NeoPixel library.
  for (int i = 0; i < NUMPIXELS; i++) {                  // set all pixels to "off" (black)
    pixels.setPixelColor(i, pixels.Color(BLACK));
  }
  pixels.show();
}

void blinkSequence(uint32_t color) {                     // the function turns the lights on and off with a 0.2 second delay in a certain color
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, color);
    }
    pixels.show();
    delay(200);
    pixels.clear();
    pixels.show();
    delay(200);
  }
}

void lightUpSequence(uint32_t color) {                   // lights up LEDS one by one
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(250);                                          // delay for the build up
  }
  delay(500);
  pixels.clear();
}

void slowLightUpSequence(uint32_t color) {               // works in a similiar way as the previous function but slower
  for (int i = 0; i < 3; i++) {
    pixels.setPixelColor(i, pixels.Color(RED));  
    pixels.show();
    delay(500);
  }
}

void fadeInSequence() {                                  // fading effect is only in blue
  for (int b = 0; b < 256; b++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(BLUE));
    }
    pixels.show();
    delay(5);
  }
}

void biDirectionalSequence() {                           // function for the lights to fill "up and down"
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(PINK));
    pixels.show();
    delay(25);
  }
  for (int i = 10; i >= 0; i--) {
    pixels.setPixelColor(i, pixels.Color(YELLOW));       // done with different(contrasting) colors for the two directions
    pixels.show();
    delay(25);
  }
}

void levelOneMode(uint32_t color) {
  color = pixels.Color(RED);
  slowLightUpSequence(color);
  blinkSequence(color);
  delay(150);                                            // The amount of time between each individually lit up pixel
}

void levelTwoMode(uint32_t color) {
  color = pixels.Color(BLUE);
  lightUpSequence(color);
  delay(100);
  blinkSequence(color);
  delay(100);
  fadeInSequence();
}

void levelThreeMode(uint32_t color) {                    // for the "ultimate love match"
  color = pixels.Color(PINK);  
  blinkSequence(color);
  biDirectionalSequence();
}

void startLightAnimation(int level) {                            
  uint32_t color;
  if (level == 1) {
    for (int i = 0; i < level; i++) {
      levelOneMode(color);
    }
  } else if (level == 2) {
    for (int i = 0; i < level; i++) {
      levelTwoMode(color);
    }
  } else if (level == 3) {
    for (int i = 0; i < level; i++) {
      levelThreeMode(color);
    }
  }
}
