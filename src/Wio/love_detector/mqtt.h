#ifndef MQTT_H
#define MQTT_H


#include "rpcWiFi.h"
#include <ArduinoMqttClient.h>
#include "led.h"

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

const char test[] = "test";
const char payload_start[]  = "Start button has been clicked";
const char payload_stop[] = "Stop button has been clicked";
const char payload_nextQ[]  = "Change to the next question";

//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;


extern void onMqttMessage(int messageSize);
extern void MQTTsubscribe(String topic);

void setupWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  printMessage("Attempting to connect to:");
  printMessage(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    printMessage(".");
    delay(5000);
  }

  printNewMessage("Connection successful.");

  delay(2000);
}

void setupMQTT(){
  setupWifi();

  printNewMessage("Connecting to MQTT Broker:");
  printMessage(broker);

  while (!mqttClient.connect(broker, port)) {
    printNewMessage("MQTT connection failed! Error code = ");
    printMessage(String(mqttClient.connectError()));
    printNewMessage("\nRetrying...");

    delay(5000);
  }
  
  //mqttClient.setKeepAliveInterval(60); // Set the keep-alive interval to 60 seconds
  printNewMessage("You're connected to the MQTT broker!");

  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(test);
  MQTTsubscribe(topic_matchResult);

  Serial.println("Serial is ready.");
  delay(2000);
}

void maintainMQTTConnection(){
  //mqttClient.poll() "keeps alive" connection between the broker and client
  //by periodically pinging the broker
  mqttClient.poll();
}

bool MQTTpublishCheck(){
  // method to check if there has passed enough time since the last 
  // MQTT publish 
  unsigned long currentMillis = 0;
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
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

void MQTTsubscribe(String topic){
  Serial.println("Subscribing to topick: ");
  Serial.println(topic);
  
  mqttClient.subscribe(topic);
}

void onMqttMessage(int messageSize){
  Serial.println("Received message");
  String newTopic = mqttClient.messageTopic();
  char* topic = &newTopic[0];
  Serial.println(topic);
  Serial.println(newTopic);
  Serial.println(topic_matchResult);
  Serial.println(strcmp(topic, topic_matchResult));

  if(!strcmp(topic, topic_matchResult)){
    int result = mqttClient.read() - (int) '0';
    Serial.println(result);
    light(result);
  }
}



#endif