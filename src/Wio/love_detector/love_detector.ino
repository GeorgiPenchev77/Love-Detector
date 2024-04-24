#include <TFT_eSPI.h>
#include "secrets.h"
TFT_eSPI tft;


#define STANDARD_HORIZONTAL_VIEW 3
//define the three buttons of the terminal: 3 is left, 2 is middle, 1 is right
#define START BUTTON_3
#define NEXT_Q BUTTON_2
#define HELP BUTTON_1


const String start_message = "Hello and Welcome to The Love Detector.\n Press the left button to begin. While in test, press the left button to stop the test or the middle button to change to the next question. Press the right button for help.";
const String result_message = "Your heart rate is: ";
const String loading_message = "Heart rate test has begun.\n You will be notified when it is complete or if there is an issue.";
const String error_message = "Heart rate measure error, test will restart automatically.\n Make sure the sensor is attached securely!";
const String reset_message = "Test has been stopped.\n Press button again to reset the test.";

const int max_heartpluse_duty = 2000;  //maximum delay between sensor logs, i.e. if >2 seconds there will be an error pop-up

unsigned long temp[21];
unsigned char counter = 0;
unsigned int heart_rate;

bool previous_state = false;

volatile bool data_effect = true;  // boolean to store whether data is valid or not
volatile bool is_started = false;  // boolean to store whether the test has been started or not (this is the via BUTTON_3)

unsigned long temp2[21];
unsigned char counter2 = 0;
unsigned int heart_rate2;

volatile bool data_effect2 = true;  // boolean to store whether data is valid or not


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/*char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.130";
int        port     = 1883;
const char topic[]  = "test";
const char topic2[]  = "real_unique_topic_2";
const char topic3[]  = "real_unique_topic_3";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;
*/


void printNewMessage(String string) {
  clear();
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.println(string);
}

void printMessage(String string){
  tft.println(string);
}



void clear() {
  tft.fillScreen(TFT_WHITE);
}

//function to read every button press to start/stop the test
void press() {
  if (digitalRead(START) == LOW) {
    is_started = !is_started;
  }
}

void reset() {
  data_effect = 0;
  counter = 0;
}

void reset_second(){
  data_effect2=0;
  counter2=0;
}

void setup() {


  pinMode(PIN_WIRE_SCL, INPUT); //Defined SCL of I2C port as Digital Input
  pinMode(D0, INPUT);
  pinMode(START, INPUT); //initialize the button as an input device
  tft.begin();
  Serial.begin(9600);
  while(!Serial);

  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen
  Serial.clear();
  Serial.println("Start");

/*
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  printNewMessage("Attempting to connect to ");
  printNewMessage(ssid);
  

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    printMessage(".");
    delay(5000);
  }

  printNewMessage("Connection successful.");

  delay(2000);

  printNewMessage("Connecting to MQTT Broker: ");
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

*/
  
  printNewMessage(start_message);

  arrayInit();  //initialize the data array with 0s
  /*
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(0)" shows that we're monitoring changes on Digital Pin 0(D0)
        - "interrupt" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
  */
  attachInterrupt(digitalPinToInterrupt(D0), interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_WIRE_SCL), interrupt_second, RISING);
  attachInterrupt(digitalPinToInterrupt(START), press, CHANGE);
}

//the loop will only change if there is a button press
void loop() {

  //mqttClient.poll();
  if (is_started != previous_state) {
    if (is_started) {
      clear();
      printNewMessage(loading_message);

    } else if (!is_started) {
      clear();
      printNewMessage(reset_message);
    }

    previous_state = is_started;
  }
}

/* interrupt function is stopped via button press,
when stopped the test will be marked as invalid and reset to run again,
it will start running on next button press*/
void interrupt() {

  if (is_started) {
    temp[counter] = millis();

    switch (counter) {
      case 0:
        break;
      default:
        unsigned long sub = temp[counter] - temp[counter - 1];
        Serial.println(sub);
        if (sub > max_heartpluse_duty) {
          data_effect = false;
          counter = 0;
          clear();
          printNewMessage(error_message);
          arrayInit();
          return;
        }
        break;
    }

    if (counter == 20 && data_effect) {
      counter = 0;
      sum();
      Serial.println("2");
    } else if (counter != 20 && data_effect) {
      counter++;
    } else {
      counter = 0;
      data_effect = true;
    }

  }

  else if (!is_started) {
    reset();
  }
}

//initializes the temp array
void arrayInit() {
  for (unsigned char i = 0; i < 20; i++) {
    temp[i] = 0;
  }
  temp[20] = millis();
}

//calculates the heart rate over 20 readings from the sensor and prints it to the screen
void sum() {
  if (data_effect) {
    heart_rate = 1200000 / (temp[20] - temp[0]);
    clear();
    printNewMessage(result_message + String(heart_rate));
    Serial.print("1: ");
    Serial.print(heart_rate);
  }
  data_effect = true;
}

void interrupt_second() {

  if (is_started) {
    temp2[counter] = millis();

    switch (counter2) {
      case 0:
        break;
      default:
        unsigned long sub1 = temp2[counter2] - temp2[counter2 - 1];
        Serial.println(sub1);
        if (sub1 > max_heartpluse_duty) {
          data_effect2 = false;
          counter2 = 0;
          clear();
          printNewMessage(error_message);
          arrayInit_second();
          return;
        }
        break;
    }

    if (counter2 == 20 && data_effect2) {
      counter2 = 0;
      sum_second();
      Serial.println("1");
    } else if (counter2 != 20 && data_effect2) {
      counter2++;
    } else {
      counter2 = 0;
      data_effect2 = true;
    }

  }

  else if (!is_started) {
    reset_second();
  }
}

//initializes the temp array
void arrayInit_second() {
  for (unsigned char i = 0; i < 20; i++) {
    temp2[i] = 0;
  }
  temp2[20] = millis();
}

//calculates the heart rate over 20 readings from the sensor and prints it to the screen
void sum_second() {
  if (data_effect2) {
    heart_rate2 = 1200000 / (temp2[20] - temp2[0]);
    tft.setCursor(200,80);
    tft.print(result_message + String(heart_rate2));
    Serial.print("2: ");
    Serial.print(heart_rate2);
  }
  data_effect2 = true;
}


