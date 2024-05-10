#include "util.h"

TFT_eSPI tft;

/* ------------------------------ Wio printing ------------------------------ */                                                                                                                 
void printMessage(String string){
  clearScreen();
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  drawCustomString(string, 2, 10, 60);
}

void printNewMessage(String string) {
  clearScreen();
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  drawCustomString(string, 2, 10, 60);
}

void setupWioOutput(){
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen
  drawHeader();
  Serial.begin(9600);
}

void drawHeader(){
  tft.fillScreen(TFT_PINK);
  tft.fillRect(0, 0, 320, 50, TFT_PURPLE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  char* text = "Love Detector";
  tft.drawString(text, getCenter(text,3), 14);
}

int getCenter(char* text, int textSize) {  
  int pixelWidth;              
  if(textSize == 2) {
    pixelWidth = CHAR_WIDTH_2;
  } else if(textSize == 3) {
    pixelWidth = CHAR_WIDTH_3;
  }
  int textWidth = strlen(text) * pixelWidth;         // get text pixel size by multiplying (number of characters in text) x (number of pixels per character)
  int xPosition = (TFT_WIDTH - textWidth) / 2;       // calculate x starting position by (width of screen) - (text pixel size) / 2
  return xPosition;
}

void clearScreen(){
   tft.fillRect(0,50,320,TFT_HEIGHT - 50, TFT_PINK);
}

void drawCustomString(String string, int textSize, int x, int y){
   int pixelWidth;              
   char* parted;
   char* text = &string[0];
   int counter;
   int currPosition = 0;
   String partedString;

  if(textSize == 2) {
    pixelWidth = CHAR_WIDTH_2;
  } else if(textSize == 3) {
    pixelWidth = CHAR_WIDTH_3;
  }
  int textWidth = strlen(text) * pixelWidth;
  int splitIndex = (TFT_WIDTH - 10) / pixelWidth;

  if(textWidth > (TFT_WIDTH-10)){
    while(splitIndex <= textWidth){
      partedString = string.substring(currPosition, splitIndex);
      parted = &partedString[0];
      currPosition = splitIndex;
      splitIndex += splitIndex;
      tft.drawString(parted, x, y);
      y += 30;
    }
  }
  else{
      tft.drawString(text, x, y);
  }
}
/* -------------------------------------------------------------------------- */