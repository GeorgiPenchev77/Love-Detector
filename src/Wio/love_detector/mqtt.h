#ifndef MQTT_H
#define MQTT_H


#include "rpcWiFi.h"
#include <ArduinoMqttClient.h>

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

const char payload_start[]  = "Start button has been clicked";
const char payload_stop[] = "Stop button has been clicked";
const char payload_nextQ[]  = "Change to the next question";

//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;




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


#endif