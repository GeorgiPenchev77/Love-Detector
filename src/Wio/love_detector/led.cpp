#include "led.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);


void neoPixelSetup(){
  //Neopixels Setup
  randomSeed(analogRead(LED_PIN));
  pixels.setBrightness(255);
  pixels.begin(); // This initializes the NeoPixel library.

  // Set all pixels to "off" (black)
  for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(BLACK));
  }
  pixels.show();

}

void blinkSequence(uint32_t color){// makes it on off then on 2 times, with small delay
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
void lightUpSequence(uint32_t color) {//lights up LEDS one by one
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(250); // Delay for the build up
  }
  delay(500);
  pixels.clear(); 
}
void fadeInSequence() {//Fading effect only blue
    for (int b = 0; b < 256; b++) {
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, b, b)); // Fade in blue
      }
      pixels.show();
      delay(5); 
    }
}
void upAndDown() { // function for the lights fillup and down 
  for (int i = 0; i < NUMPIXELS; i++) { 
      pixels.setPixelColor(i, pixels.Color(PINK)); //pink color
      pixels.show();
      delay(25);
    }
    for (int i = 10; i >= 0; i--) {
      pixels.setPixelColor(i, pixels.Color(YELLOW)); //brighter color
      pixels.show();
      delay(25);
    }
}
void slowFillUp(uint32_t color) {
  for (int i = 0; i < 3; i++) { 
      pixels.setPixelColor(i, pixels.Color(RED)); //pink(ish) color
      pixels.show();
      delay(500);
    }
}
void noMatchMode(uint32_t color) {
  color = pixels.Color(RED);
  slowFillUp(color); 
  blinkSequence(color);
  delay(150); // The amount of time between each individually lit up pixel  
}

void normalMode(uint32_t color){
  color = pixels.Color(BLUE);
  lightUpSequence(color);
  delay(100);
  blinkSequence(color);
  delay(100);
  fadeInSequence();
}
void crazyMode() { // for ultimate love match
  uint32_t color = pixels.Color(PINK); // pink color for the blinking
  blinkSequence(color);
  upAndDown();
}


void light(int level) {
  uint32_t color;
  if (level <= 25) {
    for (int i = 0; i < level; i++) {
      noMatchMode(color);
    }
  } else if (level > 25 && level <= 75) {
    for (int i = 0; i < level; i++) {
      normalMode(color);
   }
  } else {
    for (int i = 0; i < level; i++) {
      crazyMode();
   }
  }
}
