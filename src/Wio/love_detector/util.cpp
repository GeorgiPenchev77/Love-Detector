/* --------------------------------------------------------------------------- */
// Acknowledge for parts this code should be attributed to:
// The below functions: getCenter() and drawHeader() were adapted from:
// https://github.com/lakshanthad/Wio_Terminal_Classroom_Arduino/tree/main/Classroom%2012/Smart_Garden
/* --------------------------------------------------------------------------- */

#include "util.h"                                                      // include corresponding header file
TFT_eSPI tft;                                                          // initialize screen object

/* ------------------------------ Wio printing ------------------------------ */     

void setupWioOutput(){
  Serial.begin(9600);                                                  // start the serial
  tft.begin();                                                         // start the screen
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);                           // set up commands to display messages on the Wio screen
  drawHeader();
  tft.setFreeFont(FSSBO12);                                            // set font to custom font from file Free_Fonts.h
  tft.setTextSize(1);                                                  // set text size for messages to 1
}

int getPixelWidth(int textSize){                                       // get pixel width of a single character based on textSize 
  if(textSize == 2) {
    return CHAR_WIDTH_2;
  } 
  else if(textSize == 3) {
    return CHAR_WIDTH_3;
  }
}

/*
  fill in only the part of the screen, used for displaying messages,
  header part of the screen will remain untouched.
*/
void clearScreen(){ 
   tft.fillRect(0, HEADER_HEIGHT, TFT_WIDTH, TFT_HEIGHT - HEADER_HEIGHT, TFT_PINK); 
}


void printNewMessage(String text){
  clearScreen();
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(0,75);                                                 // set cursor to appropriate position to print messages(horizontal: 0px, vertical: 75px)
  tft.println(text);
}

/*
  in order to get the length of a string we need it to be an array of characters.
  Using it the function calculates the position of where "centered" text 
  should be, based on size of the text.
*/
int getCenter(char* text, int textSize){  
  int pixelWidth = getPixelWidth(textSize);           
  int textWidth = strlen(text) * pixelWidth;                           // get "width of text" by multiplying the length of the string and the width of single letter
  int xPosition = (TFT_WIDTH - textWidth) / 2;       
  return xPosition;
}

void drawHeader(){ 
  tft.fillScreen(TFT_PINK);                                           // function to draw the "Love Detector" slogan on the top of the screen
  tft.fillRect(0, 0, TFT_WIDTH, HEADER_HEIGHT, TFT_PURPLE);           // fill header part of screen with appropriate color
  tft.setFreeFont(FSSBO9);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  char* text = "Love Detector";
  tft.drawString(text, getCenter(text,3), 5);
}

/* -------------------------------------------------------------------------- */