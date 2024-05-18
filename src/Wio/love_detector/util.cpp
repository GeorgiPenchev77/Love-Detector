/* --------------------------------------------------------------------------- */
// Acknowledge for parts this code should be attributed to:
// The below functions: getCenter() and drawHeader() were adapted from:
// https://github.com/lakshanthad/Wio_Terminal_Classroom_Arduino/tree/main/Classroom%2012/Smart_Garden
/* --------------------------------------------------------------------------- */

#include "util.h"                                                      // include corresponding header file
TFT_eSPI tft;                                                          // initialize screen object
String displayedText = "";                                             // variable that stores the current text that is to be displayed
bool hasTextChanged = false;                                           // stores the state of the screen        

/* ------------------------------ Wio printing ------------------------------ */     

void setupWioOutput(){
  Serial.begin(9600);                                                  // start the serial
  tft.begin();                                                         // start the screen
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);                           // set up commands to display messages on the Wio screen
  drawHeader();
  tft.setFreeFont(FSSBO12);                                            // set font to custom font from file Free_Fonts.h
  tft.setTextSize(1);                                                  // set text size for messages to 1
}

void renderWioText(){                                                  // if text has to be changed print the needed current message 
  if(hasTextChanged){
    printNewMessage(displayedText);
    hasTextChanged = false;
  }
}

void updateWioText(String newText){                                    // if the text to be displayed is different than the currently displayed one -> change it
  if(displayedText != newText){
    displayedText = newText;
    hasTextChanged = true;
  }
}

int getPixelWidth(int textSize){                                       // get pixel width of a single character based on textSize 
  if(textSize == 2) {
    return CHAR_WIDTH_2;
  } 
  else if(textSize == 3) {
    return CHAR_WIDTH_3;
  }
}

void clearScreen(){                                                    // clear only the part of the screen where text is displayed dynamically, 
   tft.fillRect(0, HEADER_HEIGHT, TFT_WIDTH,                           // the header part of the text will remain displayed always
   TFT_HEIGHT - HEADER_HEIGHT, TFT_PINK); 
}

void printNewMessage(String text){                                     // when a new message is printed the screen is cleared before
  clearScreen();
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(0,75);                                                 // set cursor to appropriate position to print messages(horizontal: 0px, vertical: 75px)
  tft.println(text);
}

int getCenter(char* text, int textSize){                               // in order to get the length of a string we need it to be an array of characters.
  int pixelWidth = getPixelWidth(textSize);                            // using it the function calculates the position of where "centered" text should be, based on size of the text.     
  int textWidth = strlen(text) * pixelWidth;                           // get "width of text" by multiplying the length of the string and the width of single letter
  int xPosition = (TFT_WIDTH - textWidth) / 2;       
  return xPosition;
}

void drawHeader(){                                                     // function to draw the "Love Detector" slogan on the top of the screen
  tft.fillScreen(TFT_PINK);                                            
  tft.fillRect(0, 0, TFT_WIDTH, HEADER_HEIGHT, TFT_PURPLE);            // fill header part of screen with appropriate color
  tft.setFreeFont(FSSBO9);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  char* text = "Love Detector";
  tft.drawString(text, getCenter(text,3), 5);
}

/* ---------------------------- General utilities --------------------------- */

int parseInt(String string){
  int num = 0;
  for(int i = 0; i < string.length(); ++i){
    num = (num * 10) + (int) string[i] - (int) '0';
  }
  return num;
}

/* -------------------------------------------------------------------------- */