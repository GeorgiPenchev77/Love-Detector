#include "util.h"
#include "mqtt.h"
#include "heartBeatSensor.h"

/* --------------------- Hearbeat-beat sensors variables -------------------- */

volatile bool previousState = false; //boolean to store the previous state of the program
volatile bool isStarted = false;  // boolean to store whether the test has been started or not (this is the via BUTTON_3)

//all used variables are duplicated in order to get the second sensor working
unsigned long sub1, sub2;
unsigned long temp1[21], temp2[21];
unsigned char counter1, counter2;
unsigned int heart_rate1 = 0, heart_rate2 = 0;
volatile bool data_effect1 = true, data_effect2 = true;  // boolean to store whether data is valid or not

/* -------------------------------------------------------------------------- */

//please enter your sensitive data in the Secret tab ./secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = SECRET_IP;
int        port     = 1883;
const char topic[]  = "test";
const char topic2[]  = "real_unique_topic_2";
const char topic3[]  = "real_unique_topic_3";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;



//function to read every button press to start/stop the test
void press() {
  if (digitalRead(START) == LOW) {
    isStarted = !isStarted;
  }
}

/* -------------------------------------------------------------------------- */

void setup() {

  pinMode(START, INPUT);    //initialize the button as an input device
  setupOutput();
  
  
  
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
  
  printNewMessage("Connecting to MQTT Broker:");
  printMessage(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  printNewMessage("You're connected to the MQTT broker!");

  delay(2000);

  mqttClient.beginMessage(topic);
  mqttClient.print("test message from Wio");
  mqttClient.endMessage();

  printNewMessage(START_MESSAGE);

  /*
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(0)" shows that we're monitoring changes on Digital Pin 0(D0)
        - "interrupt" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
  */
  // attachInterrupt(SENSOR1, sensor1.interrupt, RISING); // Sensor 1 - left port of the Wio terminal
  // attachInterrupt(SENSOR2, sensor2.interrupt, RISING); // Sensor 2 - right port of the Wio terminal

  leftSensor.setup();
  rightSensor.setup();
  attachInterrupt(START, press, CHANGE);
}

/* -------------------------------------------------------------------------- */

//the code in the loop will only execute on button press
void loop() {

  mqttClient.poll();//keeps the connection alive

  if (isStarted != previousState) { //interchange messages based on whether test is started or not
    if (isStarted) {
      printNewMessage(LOADING_MESSAGE);
    } else if (!isStarted) {
      printNewMessage(RESET_MESSAGE);
    }
  
  previousState = isStarted; //save the current value in order to check later whether there has been a change or not
  }
}


                                                                  //From here on all functions are doubled to account for the two sensors working in the system//
/* -------------------------------------------------------------------------- */

/*interrupt functions are event listeners that execute everytime the sensor
gets a reading(eg beat of the users heart), the time of these readings is stored
in an array which is used to calculate the heart rate.*/



