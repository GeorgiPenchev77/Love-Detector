#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"
#include "led.h"

volatile bool previousState = false; // store the previous state of the START_DATE button
volatile bool isStarted = false;     // store whether the test has been started or not (this is the via S)
bool startButtonClicked = false;
bool nextQuestionClicked = false;
bool stopButtonClicked = false;

//function to read every button press to start/stop the test
void startMeasuring() {
    isStarted = true;
    // MQTT flag
    startButtonClicked = true;
}

void changeQuestion(){
  // MQTT flag
  nextQuestionClicked = true;
}

void stopMeasuring(){
  isStarted = false;
  //MQTT flag
  stopButtonClicked = true;
}


void setup() {
  // initialization of buttons as an input devices
  pinMode(START, INPUT);    
  pinMode(NEXT_QUESTION, INPUT);
  pinMode(STOP, INPUT);


  setupWioOutput();            
  setupMQTT();
  neoPixelSetup();
  
  printNewMessage(START_MESSAGE);

  // interrupts attachment
  leftSensor.setup();
  rightSensor.setup();
  attachInterrupt(START, startMeasuring, FALLING);
  attachInterrupt(NEXT_QUESTION, changeQuestion, FALLING);
  attachInterrupt(STOP, stopMeasuring, FALLING);

}

/* -------------------------------------------------------------------------- */

 
void loop() {


  int messageSize = mqttClient.parseMessage();
  if(messageSize){
    Serial.print("A message received");
    onMqttMessage(messageSize);
  }

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

    if(stopButtonClicked){
      stopButtonClicked = false;

      MQTTpublish(topic_stop, payload_stop);
    }

    if(nextQuestionClicked){
      nextQuestionClicked = false;

      MQTTpublish(topic_nextQ, payload_nextQ);
    }

    if(leftSensor.isUpdated){
      leftSensor.setIsUpdated();
      MQTTpublish(topic_heartRateLeft, String(leftSensor.heartRate));
    }

    if(rightSensor.isUpdated){
      rightSensor.setIsUpdated();
      MQTTpublish(topic_heartRateRight, String(rightSensor.heartRate));
    }

  }
  

}


