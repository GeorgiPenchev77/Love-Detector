#include "util.h"

TFT_eSPI tft;

/* ------------------------------ Wio printing ------------------------------ */
String displayedText = "";
bool hasTextChanged = false;

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

void setupWioOutput(){
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen

  Serial.begin(9600);
}

void renderWioText(){
  if(hasTextChanged){
    printNewMessage(displayedText);
    hasTextChanged = false;
  }
}

void updateWioText(String newText){
  if(displayedText != newText){
    displayedText = newText;
    hasTextChanged = true;
  }
}




/* -------------------------------------------------------------------------- */

/* ---------------------------- General utilities --------------------------- */
int parseInt(String string){
  int num = 0;
  for(int i = 0; i < string.length(); ++i){
    num = (num * 10) + (int) string[i] - (int) '0';
  }
  return num;
}

/* -------------------------------------------------------------------------- */