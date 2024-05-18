#ifndef UTIL_H
#define UTIL_H                                     // header guard

#include "TFT_eSPI.h"                              // import screen library  
#include "Free_Fonts.h"                            // import custom fonts file.
extern TFT_eSPI tft;                               // screen reference

/* ------------------------------ Wio Macros --------------------------------- */

#define DEBUG false                                // Debug macro that will trigger when a issue occurs

#define STANDARD_HORIZONTAL_VIEW 3                 // Terminal text allignment mode

//Terminal button macros
#define START BUTTON_3                             // Left button - Start heart rate measuring
#define NEXT_QUESTION BUTTON_2                     // Middle button - Change questions during date
#define STOP BUTTON_1                              // Right button - Stop heart rate measuring

#define TFT_HEIGHT 240                             // define LCD screen pixel height
#define TFT_WIDTH 320                              // define LCD screen pixel width
#define HEADER_HEIGHT 50                           // define height of Header banner
#define CHAR_HEIGHT_2 14                           // define pixel height of one character (size 2)
#define CHAR_WIDTH_2 12                            // define pixel width of one character (size 2)
#define CHAR_HEIGHT_3 24                           // define pixel height of one character (size 3)
#define CHAR_WIDTH_3 18                            // define pixel height of one character (size 3)

/* ------------------------ Wio Utility and Printing -------------------------- */

//function to set up the screen of the Wio Terminal and display initial state
extern void setupWioOutput();

//function to check if the text needs to be updated
extern void renderWioText();

//function to update text on the terminal screen
extern void updateWioText(String newText);

//function to get pixel width of one letter based on different text sizes
extern int getPixelWidth(int textSize);

//function to clear screen to initial state(i.e. no messages, only header)
extern void clearScreen();

//function to print a new message on the screen
extern void printNewMessage(String text);

//function to get the horizontal position of where "centered" logo should be
extern int getCenter(char* text, int textSize);

//function to draw "header" with logo
extern void drawHeader();

/* ---------------------------- General Utilities --------------------------- */

extern int parseInt(String string);

/* ------------------------------ Wio messages ------------------------------ */

#define START_MESSAGE       "Welcome! Follow the instructions in our app to get started! We hope you have a wonderful time!"
#define LOADING_MESSAGE     "Heart rate measuring has begun. Press the right button to pause it."
#define RESET_MESSAGE       "Heart rate measuring has been paused. Press the left button to start it."
#define ERROR_MESSAGE1       "A problem has occured with the left sensor and the test needs to restarted. This will happen automatically."
#define ERROR_MESSAGE2       "A problem has occured with the right sensor and the test needs to restarted. This will happen automatically."
#define RESULT_MESSAGE1      "Heart rate of left user is: "
#define RESULT_MESSAGE2      "Heart rate of right user is: "

/* -------------------------------------------------------------------------- */

#endif                                                  // end header guard