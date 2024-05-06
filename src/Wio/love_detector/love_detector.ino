#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"


volatile bool previousState = false; // store the previous state of the START button
volatile bool isStarted = false;     // store whether the test has been started or not (this is the via S)



//function to read every button press to start/stop the test
void press() {
  if (digitalRead(START) == LOW) {
    isStarted = !isStarted;
  }
}

void setup() {

  pinMode(START, INPUT);    //initialize the button as an input device
  setupWioOutput();            
  
  mqtt.setup();

  printNewMessage(START_MESSAGE);

  leftSensor.setup();
  rightSensor.setup();
  attachInterrupt(START, press, CHANGE);
}

/* -------------------------------------------------------------------------- */

//the code in the loop will only execute on button press
void loop() {

  mqtt.maintainConnection();

  if (isStarted != previousState) { //interchange messages based on whether test is started or not
    if (isStarted) {
      printNewMessage(LOADING_MESSAGE);
    } else if (!isStarted) {
      printNewMessage(RESET_MESSAGE);
    }
  
  previousState = isStarted; //save the current value in order to check later whether there has been a change or not
  }
}


