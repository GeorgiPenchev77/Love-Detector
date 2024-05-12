#ifndef UTIL_H
#define UTIL_H

#include <TFT_eSPI.h>
#include "Free_Fonts.h"
extern TFT_eSPI tft;

/* ------------------------------Wio Macros --------------------------------- */
#define STANDARD_HORIZONTAL_VIEW 3        // Terminal text allignment mode

//Terminal button macros
#define START BUTTON_3                    // Left button
#define NEXT_QUESTION BUTTON_2            // Middle button
#define STOP BUTTON_1                     // Right button

#define PINK 169,0,121
#define PURPLE  85,26,139

#define TFT_HEIGHT 240                             // define LCD screen pixel height
#define TFT_WIDTH 320                              // define LCD screen pixel width
#define CHAR_HEIGHT_2 14                           // define pixel height of one character (size 2)
#define CHAR_WIDTH_2 12                            // define pixel width of one character (size 2)
#define CHAR_HEIGHT_3 24                           // define pixel height of one character (size 3)
#define CHAR_WIDTH_3 18                            // define pixel height of one character (size 3)

/* ------------------------------ Wio messages ------------------------------ */

#define START_MESSAGE      "Welcome! Follow the instructions in our app to get started! We hope you have a wonderful time!"
#define LOADING_MESSAGE    "Heart rate measuring has begun. Press the right button to pause it."
#define RESET_MESSAGE      "Heart rate measuring has been paused. Press the left button to start it."
#define ERROR_MESSAGE      "A problem has occured and the test needs to restarted. This will happen automatically."
#define RESULT_MESSAGE    "Heart rate is: "

/* ------------------------------ Wio Printing ------------------------------ */
extern void setupWioOutput();

extern int getPixelWidth(int textSize);

extern void clearScreen();

extern void printNewMessage(String text);

extern int getCenter(char* text, int textSize);

extern void drawHeader();

//extern void drawCustomString(String text, int textSize, int x, int y);
/* -------------------------------------------------------------------------- */

#endif