#include "util.h"

TFT_eSPI tft;

/* ------------------------------ Wio printing ------------------------------ */     

void setupWioOutput(){
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen
  drawHeader();
  Serial.begin(9600);
}

int getPixelWidth(int textSize){
  if(textSize == 2) {
    return CHAR_WIDTH_2;
  } 
  else if(textSize == 3) {
    return CHAR_WIDTH_3;
  }
}

int getPixelHeight(int textSize){
  if(textSize == 2) {
    return CHAR_HEIGHT_2;
  } 
  else if(textSize == 3) {
    return CHAR_HEIGHT_3;
  }
}

void clearScreen(){
   tft.fillRect(0,50,320,TFT_HEIGHT - 50, TFT_PINK);
}

void printMessage(String string){
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  drawCustomString(string, 2, 10, 60);
}

void printNewMessage(String string) {
  clearScreen();
  printMessage(string);
}

int getCenter(char* text, int textSize) {  
  int pixelWidth = getPixelWidth(textSize);           
  int textWidth = strlen(text) * pixelWidth;         // get text pixel size by multiplying (number of characters in text) x (number of pixels per character)
  int xPosition = (TFT_WIDTH - textWidth) / 2;       // calculate x starting position by (width of screen) - (text pixel size) / 2
  return xPosition;
}

void drawHeader(){
  tft.fillScreen(TFT_PINK);
  tft.fillRect(0, 0, 320, 50, TFT_PURPLE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  char* text = "Love Detector";
  tft.drawString(text, getCenter(text,3), 14);
}

void drawCustomString(String text, int textSize, int x, int y) {
    int pixelWidth = getPixelWidth(textSize);   
    int pixelHeight = getPixelHeight(textSize); 
    int maxWidth = TFT_WIDTH - 20; // Adjust this margin as necessary
    int verticalPosition = y;
    char newWord = ' ';

    // Split the string into words
    String currentWord;
    String outputString;
    int currWidth = 0;
    for (char & c : text) {
        if (c == newWord) {
            int wordWidth = currentWord.length() * pixelWidth;
            if (currWidth + wordWidth >= maxWidth) {
                tft.drawString(outputString, x, verticalPosition);
                verticalPosition += pixelHeight;
                outputString = "";
                currWidth = 0;
            }
            outputString += currentWord;
            outputString += newWord;
            currWidth += wordWidth;
            currentWord = "";
        } else {
            currentWord += c;
        }
    }
    // Draw the remaining part of the string
    int remainingWidth = currentWord.length() * pixelWidth;
    if (currWidth + remainingWidth >= maxWidth) {
        tft.drawString(outputString, x, verticalPosition);
        verticalPosition += pixelHeight;
        outputString = "";
        currWidth = 0;
    }
    outputString += currentWord;
    tft.drawString(outputString, x, verticalPosition);
}
/* -------------------------------------------------------------------------- */