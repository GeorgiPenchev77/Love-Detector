#include "util.h"

TFT_eSPI tft;

/* ------------------------------ Wio printing ------------------------------ */
                                                                                                                                   
void printMessage(String string){
  tft.setTextSize(2);
  tft.println(string);
}

void printNewMessage(String string) {
  tft.fillScreen(TFT_WHITE);
  tft.setCursor(10,10);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.println(string);
}

/* -------------------------------------------------------------------------- */

void setupOutput(){
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen

  Serial.begin(9600);

}
