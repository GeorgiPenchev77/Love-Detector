#include "util.h"

TFT_eSPI tft;

/* ------------------------------ Wio printing ------------------------------ */     

void setupWioOutput(){
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen
  drawHeader();
  tft.setFreeFont(FSSBO12);
  tft.setTextSize(1);
}

int getPixelWidth(int textSize){
  if(textSize == 2) {
    return CHAR_WIDTH_2;
  } 
  else if(textSize == 3) {
    return CHAR_WIDTH_3;
  }
}

void clearScreen(){ // clear part of screen used for messages
   tft.fillRect(0, HEADER_HEIGHT, TFT_WIDTH, TFT_HEIGHT - HEADER_HEIGHT, TFT_PINK); 
}

void printNewMessage(String text){
  clearScreen();
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(0,75);
  tft.println(text);
}

int getCenter(char* text, int textSize){  // calculates the position of where "centered" text should be, based on text size
  int pixelWidth = getPixelWidth(textSize);           
  int textWidth = strlen(text) * pixelWidth;         
  int xPosition = (TFT_WIDTH - textWidth) / 2;       
  return xPosition;
}

void drawHeader(){ // function to draw the "Love Detector" slogan on the top of the screen
  tft.fillScreen(TFT_PINK);
  tft.fillRect(0, 0, TFT_WIDTH, HEADER_HEIGHT, TFT_PURPLE);
  tft.setFreeFont(FSSBO9);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  char* text = "Love Detector";
  tft.drawString(text, getCenter(text,3), 5);
}

/* -------------------------------------------------------------------------- */