#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"
#include "led.h"





volatile bool previousState = false; // store the previous state of the START button
volatile bool isStarted = false;     // store whether the test has been started or not (this is the via S)
bool startButtonClicked = false;
bool nextQuestionClicked = false;

//function to read every button press to start/stop the test
void press() {
  if (digitalRead(START) == LOW) {
    isStarted = !isStarted;
    //sends topic to broker on buttonpress. 
    if (isStarted){
      startButtonClicked = true;
    }
  }
}

void changeQuestion(){
  if (digitalRead(NEXT_QUESTION) == LOW){
    nextQuestionClicked = true;
  }
}


void setup() {

  pinMode(START, INPUT);    //initialize the button as an input device
  pinMode(NEXT_QUESTION, INPUT);



  setupWioOutput();            
  setupMQTT();
  neoPixelSetup();
  

  printNewMessage(START_MESSAGE);
  leftSensor.setup();
  rightSensor.setup();
  attachInterrupt(START, press, CHANGE);
  attachInterrupt(NEXT_QUESTION, changeQuestion, CHANGE);
}

/* -------------------------------------------------------------------------- */

//the code in the loop will only execute on button press
void loop() {

  maintainMQTTConnection();

  if (isStarted != previousState) { //interchange messages based on whether test is started or not
    if (isStarted) {
      printNewMessage(LOADING_MESSAGE);
    } else if (!isStarted) {
      printNewMessage(RESET_MESSAGE);
    }
  
  previousState = isStarted; //save the current value in order to check later whether there has been a change or not
  }

  
  if(MQTTpublishCheck){

    if(startButtonClicked){
      startButtonClicked = false;

      MQTTpublish(topic_start, payload_start);
    }

    if(nextQuestionClicked){
      nextQuestionClicked = false;

      MQTTpublish(topic_nextQ, payload_nextQ);
    }
  }

}


