#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"
#include "led.h"

volatile bool previousState = false;  // store the previous state of the START_DATE button
bool startButtonClicked = false;
bool nextQuestionClicked = false;
bool stopButtonClicked = false;

//function to read every button press to start/stop the test
void startMeasuring() {
  updateWioText(LOADING_MESSAGE);
  // MQTT flag
  startButtonClicked = true;
}

void changeQuestion() {
  // MQTT flag
  nextQuestionClicked = true;
}

void stopMeasuring() {
  updateWioText(RESET_MESSAGE);
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

  updateWioText(START_MESSAGE);

  // interrupts attachment
  leftSensor.setup();
  rightSensor.setup();
  attachInterrupt(START, startMeasuring, FALLING);
  attachInterrupt(NEXT_QUESTION, changeQuestion, FALLING);
  attachInterrupt(STOP, stopMeasuring, FALLING);
}

/* -------------------------------------------------------------------------- */


void loop() {

  if(DEBUG) Serial.println("Witch hunting...");


  //Update terminal text displayed
  renderWioText();

  if(DEBUG) Serial.println("Flag 1...");

  if (MQTTpublishCheck()) {
    if (startButtonClicked) {
      startButtonClicked = false;
      HBSensor::processStartClick();
      MQTTpublish(topic_start, payload_start);
    }

    if (stopButtonClicked) {
      stopButtonClicked = false;
      HBSensor::processStopClick();
      MQTTpublish(topic_stop, payload_stop);
    }

    if (nextQuestionClicked) {
      nextQuestionClicked = false;

      MQTTpublish(topic_nextQ, payload_nextQ);
    }


    if (HBSensor::isIMStarted) {
      if(DEBUG) Serial.println("Check if IM is ready...");
      if (HBSensor::isIMReady()) {
        String topic;
        switch (HBSensor::currentIMUser) {
          case LEFT:
            topic = topic_heartRateLeft;
            break;
          case RIGHT:
            topic = topic_heartRateRight;
            break;
        }
        if(DEBUG) Serial.println("Trying to publish...");
        MQTTpublish(topic, String(HBSensor::getIMHeartrate()));
      } else {
        if(DEBUG) Serial.println("IM isn't ready");
      }
    }

    if (HBSensor::isDateStarted) {
      int cTimer = millis();
      if (cTimer - HBSensor::timer > HBSensor::interval) {
        HBSensor::timer = cTimer;
        String message = String(leftSensor.getCurrentHeartrate()) + " " + String(rightSensor.getCurrentHeartrate());
        MQTTpublish(topic_heartRateBoth, message);
      }
    }
  }

  //Artifact after 4 hours of debugging
  if(DEBUG) Serial.println("Witches have been hunted!");
}
