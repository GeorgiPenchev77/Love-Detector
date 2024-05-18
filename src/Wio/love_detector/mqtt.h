/* --------------------------------------------------------------------------- */
// Reminder! "secrets.h" file needs to created and updated with your information,
// in order for the program to function correctly.
// The following library is used in order to implement publish and subscribe using MQTT broker:
// https://github.com/arduino-libraries/ArduinoMqttClient
/* --------------------------------------------------------------------------- */

#ifndef MQTT_H
#define MQTT_H                                                         // header guard

#include "secrets.h"                                                   // include "private" file for network info 
#include "rpcWiFi.h"                                                   // include all libraries and files needed 
#include "ArduinoMqttClient.h"
#include "led.h"
#include "heartBeatSensor.h"
#include "util.h"

/* ------------------------------ Initialization --------------------------------- */

WiFiClient wifiClient;                                                 // initialize wifiClient for connecting to internet
MqttClient mqttClient(wifiClient);                                     // initialize mqttClient for connecting to broker


const char ssid[] = SECRET_SSID;                                       // your network SSID (name)
const char pass[] = SECRET_PASS;                                       // your network password (use for WPA, or use as key for WEP)

const char broker[] = SECRET_IP;                                       // your network's ip
int        port     = 1883;                                            // standard port for running broker locally

const char topic_start[] = "start_button_click";                       // all topics that are to be published or subsrcribed
const char topic_stop[] = "stop_button_click";
const char topic_nextQ[] = "change_question";
const char topic_heartRateLeft[] = "heart_rate_left";
const char topic_heartRateRight[] = "heart_rate_right";
const char topic_matchResult[] = "match_result";
const char topic_dateStarted[] = "date_started";
const char topic_dateStopped[] = "date_stopped";
const char topic_heartRateBoth[] = "heart_rate_both";
const char topic_IMStarted[] = "im_started";
const char topic_IMStopped[] = "im_stopped";
const char topic_IMUserSwitched[] = "im_user_switched";

const char payload_start[]  = "Start button has been clicked";         // all payloads for publishing topics on button presses
const char payload_stop[] = "Stop button has been clicked";
const char payload_nextQ[]  = "Change to the next question";

const long interval = 1000;                                            // set interval for sending messages (in milliseconds)
unsigned long previousMillis = 0;                                      // time from previous message will be stored here

/* ------------------------------ Decralation of functions --------------------------------- */

extern void onMqttMessage(int messageSize);

extern void MQTTsubscribe();

extern int processMessage();

/* ------------------------------ MQTT implementation --------------------------------- */

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                                                   // disconnect from previously connected wifi 

  printNewMessage("Connection to WiFi is being established.");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {                     // retry every 5 seconds if connection failed
    printNewMessage("WiFi connection failed! Retrying...");
    delay(5000);
  }

  printNewMessage("You're connected to the WiFi network.");
  delay(2000);
}

void setupMQTT() {
  setupWifi();                                                         // setup wifi before setting up broker. Applying the same logic when connecting to broker

  printNewMessage("Connection to MQTT Broker is being established.");

  while (!mqttClient.connect(broker, port)) {
    printNewMessage("MQTT broker connection failed! Retrying...");
    delay(5000);
  }

  printNewMessage("You're connected to the MQTT broker!");

  mqttClient.onMessage(onMqttMessage);                                 // setup custom function that should trigger on message received
  MQTTsubscribe();                                                     // subscribe to all needed topics
  delay(2000);
}



void maintainMQTTConnection() {
  if (DEBUG) Serial.println("Maintaining connection...");              // mqttClient.poll() "keeps alive" connection between the broker and client
  mqttClient.poll();                                                   // by periodically pinging the broker

  if (!mqttClient.connected()) {
    Serial.println("Disconnected");
    if (mqttClient.connect(broker, port)) {
      Serial.println("reconnected");
    }
  }

  if (DEBUG) Serial.println("Connection maintained!");
}

bool MQTTpublishCheck() {                                              // method to check if there has passed enough time since the last MQTT publish
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    maintainMQTTConnection();
    return true;
  } else {
    return false;
  }
}

void MQTTpublish(String topic, String payload) {                      // method to publish a message with specified topic and payload
  mqttClient.beginMessage(topic);
  mqttClient.print(payload);
  mqttClient.endMessage();
}

void MQTTsubscribe() {                                                // method to subscribe to all the needed topics 
  Serial.println("Subscribing to topicks: ");
  Serial.println(topic_matchResult);
  Serial.println(topic_dateStarted);
  Serial.println(topic_dateStopped);
  Serial.println(topic_IMStarted);
  Serial.println(topic_IMStopped);
  Serial.println(topic_IMUserSwitched);

  mqttClient.subscribe(topic_matchResult);
  mqttClient.subscribe(topic_dateStarted);
  mqttClient.subscribe(topic_dateStopped);
  mqttClient.subscribe(topic_IMUserSwitched);
  mqttClient.subscribe(topic_IMStopped);
  mqttClient.subscribe(topic_IMStarted);
}

void onMqttMessage(int messageSize) {                                 // function that takes a MQTT message and deligates functionality
  String newTopic = mqttClient.messageTopic();                        // based on the topic
  char* topic = &newTopic[0];
  Serial.printf("Received message on topic: %s\n", topic);

  int message = processMessage();
  Serial.printf("With contents: %d\n", message);

  if (!strcmp(topic, topic_matchResult)) {                            //  strcmp returns 0 if strings are equal 
    startLightAnimation(message);
  } else if (!strcmp(topic, topic_dateStarted)) {
    HBSensor::startDate(message);
  } else if (!strcmp(topic, topic_dateStopped)) {
    HBSensor::stopDate();
  } else if (!strcmp(topic, topic_IMStarted)) {
    HBSensor::startIM(message);
  } else if (!strcmp(topic, topic_IMStopped)) {
    HBSensor::stopIM();
  } else if (!strcmp(topic, topic_IMUserSwitched)) {
    HBSensor::switchIMUser();
  }
}

int processMessage() {                                                // function to read the payload and process it into a number
  String message = "";
  while (mqttClient.available()) {
    message = message + (char)mqttClient.read();
  }
  return parseInt(message);
}

/* ---------------------------------------------------------------------------- */

#endif                                                                // end header guard