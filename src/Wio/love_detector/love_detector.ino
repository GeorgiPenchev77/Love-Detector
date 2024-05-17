#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"
#include "led.h"

/* -------------------------------------------------------------------------- */

volatile bool previousState = false;                // store the previous state of the START button
bool startButtonClicked = false;
bool nextQuestionClicked = false;
bool stopButtonClicked = false;

/* -------------------------------------------------------------------------- */

void startMeasuring() {                             // function to read every button press to start the measuring
  updateWioText(LOADING_MESSAGE);
  startButtonClicked = true;                        // an MQTT flag for publishing message
}

void changeQuestion() {                             // function to read every button press to change question during date
  nextQuestionClicked = true;                       // an MQTT flag for publishing message
}

void stopMeasuring() {                              // function to read every button press to stop the measuring
  updateWioText(RESET_MESSAGE);
  stopButtonClicked = true;                         // an MQTT flag for publishing message
}

/* -------------------------------------------------------------------------- */

void setup() {

  pinMode(START, INPUT);                            // initialization of buttons as an input devices
  pinMode(NEXT_QUESTION, INPUT);
  pinMode(STOP, INPUT);

  setupWioOutput();                                 // setup display  
  setupMQTT();                                      // setup Wifi and MQTT connections
  neoPixelSetup();                                  // setup LED lights

  updateWioText(START_MESSAGE);                     // greet user with starting message

  leftSensor.setup();                               // interrupts attachment for both sensors and all three buttons
  rightSensor.setup();
  attachInterrupt(START, startMeasuring, FALLING);
  attachInterrupt(NEXT_QUESTION, changeQuestion, FALLING);
  attachInterrupt(STOP, stopMeasuring, FALLING);

}

/* -------------------------------------------------------------------------- */

void loop() {

  renderWioText();                                  // update terminal text displayed

  if (MQTTpublishCheck()) {                         // publish only after check to prevent from issues
    
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

  if(DEBUG) Serial.println("Witches have been hunted!");                    // artefact after 4 hours of debugging :(
}
