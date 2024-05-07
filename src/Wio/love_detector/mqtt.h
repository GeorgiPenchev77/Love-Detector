#ifndef MQTT_H
#define MQTT_H

#include "rpcWiFi.h"
#include <ArduinoMqttClient.h>


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





class MQTT{
  public:

    //please enter your sensitive data in the Secret tab ./secrets.h
    const char ssid[] = SECRET_SSID;  // your network SSID (name)
    const char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

    WiFiClient wifiClient;
    MqttClient mqttClient(wifiClient);

    const char broker[] = SECRET_IP;
    int        port     = 1883;
    const char topic_start[]  = "startbutton_click";
    const char topic_nextQ[]  = "change_question";
    const char topic3[]  = "real_unique_topic_3";

    const char payload_start[]  = "Start button has been clicked";
    const char payload_nextQ[]  = "Change to the next question";


    //set interval for sending messages (milliseconds)
    const long interval = 1000;
    unsigned long previousMillis = 0;
    unsigned long currentMillis = 0;

    void setup(){
      setupWifi();
      setupMQTT();
    }

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
      printNewMessage("Connecting to MQTT Broker:");
      printMessage(broker);

      while (!mqttClient.connect(broker, port)) {
        printNewMessage("MQTT connection failed! Error code = ");
        printMessage(mqttClient.connectError());
        printNewMessage("\nRetrying...");

        delay(5000);
      }
      printNewMessage("You're connected to the MQTT broker!");

      delay(2000);
    }

    void maintainConnection(){
      mqttClient.poll();
    }

    void publish(String topic, String payload){
        mqttClient.beginMessage(topic);
        mqttClient.print(payload);
        mqttClient.endMessage();
    }
}

MQTT mqtt;

#endif