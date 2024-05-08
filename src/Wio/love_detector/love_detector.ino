#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"
#include "led.h"

volatile bool previousState = false; // store the previous state of the START_DATE button
volatile bool isStarted = false;     // store whether the test has been started or not (this is the via S)
bool startButtonClicked = false;
bool nextQuestionClicked = false;
bool individualMeasurementClicked = false;

//function to read every button press to start/stop the test
void startSpeedDate() {
    isStarted = !isStarted;

    // MQTT flag
    if (isStarted){
      startButtonClicked = true;
    }
}

void changeQuestion(){
  // MQTT flag
  nextQuestionClicked = true;
}

void startIndividualMeasurement(){
  //MQTT flag
  individualMeasurementClicked = true;
}


void setup() {
  // initialization of buttons as an input devices
  pinMode(START_DATE, INPUT);    
  pinMode(NEXT_QUESTION, INPUT);
  pinMode(INDIVIDUAL_MEASUREMENT, INPUT);


  setupWioOutput();            
  setupMQTT();
  neoPixelSetup();
  
  printNewMessage(START_MESSAGE);

  // interrupts attachment
  leftSensor.setup();
  rightSensor.setup();
  attachInterrupt(START_DATE, startSpeedDate, FALLING);
  attachInterrupt(NEXT_QUESTION, changeQuestion, FALLING);
  attachInterrupt(INDIVIDUAL_MEASUREMENT, startIndividualMeasurement, FALLING);

}

/* -------------------------------------------------------------------------- */

 
void loop() {

  // change message based on whether test is started or not
  if (isStarted != previousState) { 
    if (isStarted) {
      printNewMessage(LOADING_MESSAGE);
    } else if (!isStarted) {
      printNewMessage(RESET_MESSAGE);
    }
    //save the current value in order to check later whether there has been a change or not
    previousState = isStarted; 
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

    if(individualMeasurementClicked){
      individualMeasurementClicked = false;

      MQTTpublish(topic_individualM, payload_individualM);
    }

    if(leftSensor.isUpdated){
      leftSensor.setIsUpdated();
      MQTTpublish(topic_heartRateLeft, String(leftSensor.heartRate));
      Serial.println("Left Success");
    }

    if(rightSensor.isUpdated){
      rightSensor.setIsUpdated();
      MQTTpublish(topic_heartRateRight, String(rightSensor.heartRate));
      Serial.println("Right Success");
    }

  }
  

}


