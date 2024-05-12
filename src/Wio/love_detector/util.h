#ifndef UTIL_H
#define UTIL_H

#include <TFT_eSPI.h>
extern TFT_eSPI tft;

/* ------------------------------Wio Macros --------------------------------- */
#define STANDARD_HORIZONTAL_VIEW 3 // Terminal text allignment mode

//Terminal button macros
#define START BUTTON_3               // Left button
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
/* -------------------------------------------------------------------------- */

/* ------------------------------ Wio Printing ------------------------------ */
extern void setupWioOutput();

extern int getPixelWidth(int textSize);

extern void clearScreen();

extern void printMessage(String text);

extern void printNewMessage(String text);

extern int getCenter(char* text, int textSize);

extern void drawHeader();

extern void drawCustomString(String text, int textSize, int x, int y);
/* -------------------------------------------------------------------------- */

/* ------------------------------ Wio messages ------------------------------ */
#define START_MESSAGE   "Welcome! Follow the instructions to get started! We hope you have a wonderful time!"
#define RESULT_MESSAGE1 "Heart rate of left user is: "
#define RESULT_MESSAGE2 "Heart rate of right user is: "
#define LOADING_MESSAGE "Heart rate test has begun. You will be notified when it is complete or if there is an issue."
#define ERROR_MESSAGE1  "Heart rate of left user measure error, test will restart automatically. Make sure the sensor is attached securely!"
#define ERROR_MESSAGE2  "Heart rate of right user measure error, test will restart automatically. Make sure the sensor is attached securely!"
#define RESET_MESSAGE   "Test has been stopped. Press button again to reset the test."
/* -------------------------------------------------------------------------- */



#endif