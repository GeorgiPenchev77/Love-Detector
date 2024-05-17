#ifndef MQTT_H
#define MQTT_H


#include "rpcWiFi.h"
#include <ArduinoMqttClient.h>
#include "led.h"
#include "heartBeatSensor.h"
#include "util.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

/*
secrets.h is file used to store sensitive network data.
Create a file with following content:

#define SECRET_SSID "NETWORK"
#define SECRET_PASS "PASSWORD"
#define SECRET_IP   "0.0.0.0"

changing NETWORK to the name of the network the Wio is connecting to,
         PASSWORD to the actual password of that network and
         0.0.0.0 to ip of your MQTT borker

*/
#include "secrets.h"


//please enter your sensitive data in the Secret tab ./secrets.h
const char ssid[] = SECRET_SSID;  // your network SSID (name)
const char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

const char broker[] = SECRET_IP;
int        port     = 1883;

const char topic_start[] = "start_button_click";
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


const char test[] = "test";
const char payload_start[]  = "Start button has been clicked";
const char payload_stop[] = "Stop button has been clicked";
const char payload_nextQ[]  = "Change to the next question";

//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;


extern void onMqttMessage(int messageSize);
extern void MQTTsubscribe();
extern int processMessage();

void setupWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  printNewMessage("Connection is being established.");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    printNewMessage("Retrying...");
    delay(5000);
  }

  printNewMessage("Connection successful.");

  delay(2000);
}

void setupMQTT(){
  setupWifi();

  printNewMessage("Connection to MQTT Broker is being established.");

  while (!mqttClient.connect(broker, port)) {
    printNewMessage("MQTT broker connection failed! Retrying...");

    delay(5000);
  }
  
  //mqttClient.setKeepAliveInterval(60); // Set the keep-alive interval to 60 seconds
  printNewMessage("You're connected to the MQTT broker!");

  mqttClient.onMessage(onMqttMessage);
  MQTTsubscribe();
  delay(2000);
}



void maintainMQTTConnection(){
  if(DEBUG) Serial.println("Maintaining connection...");
  //mqttClient.poll() "keeps alive" connection between the broker and client
  //by periodically pinging the broker
  mqttClient.poll();

  if (!mqttClient.connected()) {
    Serial.println("Disconnected");
    if (mqttClient.connect(broker, port)) {
      Serial.println("reconnected");
    }
  }

  if(DEBUG) Serial.println("Connection maintained!");
}

bool MQTTpublishCheck(){


  // method to check if there has passed enough time since the last 
  // MQTT publish 
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    maintainMQTTConnection();
    return true;
  } else {
    return false;
  }
}

void MQTTpublish(String topic, String payload){
    mqttClient.beginMessage(topic);
    mqttClient.print(payload);
    mqttClient.endMessage();
}

void MQTTsubscribe(){
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

void onMqttMessage(int messageSize){

  String newTopic = mqttClient.messageTopic();
  char* topic = &newTopic[0];
  Serial.printf("Received message on topic: %s\n", topic);


  int message = processMessage();
  Serial.printf("With contents: %d\n", message);

  //strcmp returns 0 if strings are equal
  if(!strcmp(topic, topic_matchResult)){
    light(message);
  } else if(!strcmp(topic, topic_dateStarted)){
    HBSensor::startDate(message);
  } else if(!strcmp(topic, topic_dateStopped)){
    HBSensor::stopDate();
  } else if(!strcmp(topic, topic_IMStarted)){
    HBSensor::startIM(message);
  } else if(!strcmp(topic, topic_IMStopped)){
    HBSensor::stopIM();
  } else if(!strcmp(topic, topic_IMUserSwitched)){
    HBSensor::switchIMUser();
  }
}

int processMessage(){
  String message = "";
  while(mqttClient.available()){
    message = message + (char) mqttClient.read();
  }
  return parseInt(message);
}



#endif