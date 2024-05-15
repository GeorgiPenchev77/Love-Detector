#ifndef UTIL_H
#define UTIL_H

#include <TFT_eSPI.h>
extern TFT_eSPI tft;

/* ------------------------------Wio Macros --------------------------------- */
#define STANDARD_HORIZONTAL_VIEW 3 // Terminal text allignment mode

//Terminal button macros
#define START BUTTON_3               // Left button
#define NEXT_QUESTION BUTTON_2            // Middle button
#define STOP BUTTON_1   // Right button
/* -------------------------------------------------------------------------- */

/* ------------------------------ Wio Printing ------------------------------ */
extern void setupWioOutput();

extern void printMessage(String string);

extern void printNewMessage(String string);

extern void renderWioText();

extern void updateWioText(String newText);
/* -------------------------------------------------------------------------- */

/* ------------------------------ Wio messages ------------------------------ */
#define START_MESSAGE   "Hello and Welcome to The Love Detector.\nPress the \
left button to begin. While in test, press the left\
button to stop the test or the middle button to\
change to the next question. Press the right button for help."
                              

#define RESULT_MESSAGE1 "Heart rate of left user is: "
#define RESULT_MESSAGE2 "Heart rate of right user is: "
#define LOADING_MESSAGE "Heart rate test has begun.\n You will be notified \
when it is complete or if there is an issue."

#define ERROR_MESSAGE1  "Heart rate of left user measure error, test will \
restart automatically.\n Make sure the sensor is \
attached securely!"

#define ERROR_MESSAGE2  "Heart rate of right user measure error, test will \
restart automatically.\n Make sure the sensor is \
attached securely!"

#define RESET_MESSAGE   "Test has been stopped.\n Press button again to reset the test."
/* -------------------------------------------------------------------------- */

/* ---------------------------- General Utilities --------------------------- */
extern int parseInt(String string);

/* -------------------------------------------------------------------------- */

#define DEBUG false

#endif