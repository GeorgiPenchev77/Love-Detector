#include <TFT_eSPI.h>
#include "rpcWiFi.h" 
#include <ArduinoMqttClient.h>
#include "secrets.h"
TFT_eSPI tft;

//NeoPixels
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#endif


#define STANDARD_HORIZONTAL_VIEW 3
//define the three buttons of the terminal: 3 is left, 2 is middle, 1 is right
#define START BUTTON_3
#define NEXT_QUESTION BUTTON_2
#define HELP BUTTON_1

#define SENSOR1 PIN_WIRE_SCL //define the left pin of the terminal as Sensor 1 
#define SENSOR2 0 //define the right pin of the terminal as Sensor 2

//Neopixels
#define LED_PIN D2
#define NUMPIXELS    10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);


const String start_message = "Hello and Welcome to The Love Detector.\n Press the left button to begin. While in test, press the left button to stop the test or the middle button to change to the next question. Press the right button for help.";
const String result_message1 = "Heart rate of left user is: ";
const String result_message2 = "Heart rate of right user is: ";
const String loading_message = "Heart rate test has begun.\n You will be notified when it is complete or if there is an issue.";
const String error_message1 = "Heart rate of left user measure error, test will restart automatically.\n Make sure the sensor is attached securely!";
const String error_message2 = "Heart rate of right user measure error, test will restart automatically.\n Make sure the sensor is attached securely!";
const String reset_message = "Test has been stopped.\n Press button again to reset the test.";

const int max_heartpluse_duty = 2000;  //maximum delay between sensor logs, i.e. if >2 seconds there will be an error pop-up
const int total = 1200000; // used to calculate heart-rate
const int measure_limit = 20; //the limit of sensor measurements

volatile bool previous_state = false; //boolean to store the previous state of the program
volatile bool is_started = false;  // boolean to store whether the test has been started or not (this is the via BUTTON_3)
bool start_button_clicked = false;
bool next_question_clicked = false;

//all used variables are duplicated in order to get the second sensor working
unsigned long sub1, sub2;
unsigned long temp1[21], temp2[21];
unsigned char counter1, counter2;
unsigned int heart_rate1 = 0, heart_rate2 = 0;
volatile bool data_effect1 = true, data_effect2 = true;  // boolean to store whether data is valid or not

//--------------------------------------------------------------------------------------------------------------------------------------------------------------//

//please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = SECRET_IP;
int        port     = 1883;
const char topic[]  = "test";
const char topic2[]  = "startbutton_click";
const char topic_nextq[]  = "change_question";


//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;


//--------------------------------------------------------------------------------------------------------------------------------------------------------------//
                                                                           //Printing functions//                                                        
void printMessage(String string){
  tft.setTextSize(2);
  tft.println(string);
}


void printNewMessage(String string) {
  tft.fillScreen(TFT_WHITE);
  tft.setCursor(10,10);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.println(string);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------//
                                                                        //Publishing functions//

void pubNextQuestion(){
  if(next_question_clicked){
  next_question_clicked = false;
  mqttClient.beginMessage(topic_nextq);
  mqttClient.print("Change to next question");
  mqttClient.endMessage();
  }
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------------//

//function to read every button press to start/stop the test
void press() {
  if (digitalRead(START) == LOW) {
    is_started = !is_started;

//sends topic to broker on buttonpress. 
    if (is_started){
      start_button_clicked = true;
    }
  }
}

void chng_quest(){
  if (digitalRead(NEXT_QUESTION) == LOW){
    next_question_clicked = true;
  }
}

void reset1() {
  data_effect1 = 0;
  counter1 = 0;
  sub1 = 0;
}

void reset2() {
  data_effect2 = 0;
  counter2 = 0;
  sub2 = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void setup() {
  pinMode(SENSOR1, INPUT);  //Define the analog/digital port as a digital port (left port)
  pinMode(SENSOR2, INPUT);  //Define the right port
  pinMode(START, INPUT);    //initialize the button as an input device
  pinMode(NEXT_QUESTION, INPUT);
  tft.begin();
  Serial.begin(9600);
  
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  printMessage("Attempting to connect to:");
  printMessage(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
     //failed, retry
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
  
  //Neopixels Setup
  randomSeed(analogRead(LED_PIN));
  pixels.setBrightness(255);
  pixels.begin(); // This initializes the NeoPixel library.

    // Set all pixels to "off" (black)
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
     pixels.show();

  printNewMessage(start_message);

  arrayInit2();
  arrayInit1();
    /*
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(0)" shows that we're monitoring changes on Digital Pin 0(D0)
        - "interrupt" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
    */
  attachInterrupt(SENSOR1, interrupt1, RISING); // Sensor 1 - left port of the Wio terminal
  attachInterrupt(SENSOR2, interrupt2, RISING); // Sensor 2 - right port of the Wio terminal
  attachInterrupt(START, press, CHANGE);
  attachInterrupt(NEXT_QUESTION, chng_quest, CHANGE);
}




//the loop will only change if there is a button press
void loop() {

 mqttClient.poll();
 Serial.println(is_started);

  if (is_started != previous_state) { //interchange messages based on whether test is started or not
    if (is_started) {
      printNewMessage(loading_message);
    } else if (!is_started) {
      printNewMessage(reset_message);
    }
  
    previous_state = is_started; //save the current value in order to check later whether there has been a change or not
  }

  //Neopixels test-value
  int randomLevel = random(1, 100);
  light(randomLevel);
  delay(200);
}

/*interrupt function is stopped via button press,
when stopped the test will be marked as invalid and reset to run again,
it will start running on next button press*/
void interrupt1() {

  if (is_started) {
    temp1[counter1] = millis(); //start counting time between sensor ticks

    switch (counter1) {
      case 0:
        break;
      default:
        sub1 = temp1[counter1] - temp1[counter1 - 1];
        break;
    }

    if (sub1 > max_heartpluse_duty) { //check whether the time between the now and the last tick was greater than 2 seconds, if so reset the test as it would be invalid
      reset1();
      printNewMessage(error_message1);
      arrayInit1();
    }

    if (counter1 == measure_limit && data_effect1) {
      counter1 = 0;
      sum1();
    } else if (counter1 != measure_limit && data_effect1) {
      counter1++;
    } else {
      counter1 = 0;
      data_effect1 = true;
    }

  }

  else if (!is_started) { // reset the test if the test has been paused as data would not be valid then
    reset1();
  }
}


void interrupt2() {

  if (is_started) {
    temp2[counter2] = millis();

    switch (counter2) {
      case 0:
        break;
      default:
        sub2 = temp2[counter2] - temp2[counter2 - 1]; 
        // dont need to check in switch as it makes code more clearer
        break;
    }


    if (sub2 > max_heartpluse_duty) { //check whether the time between the now and the last tick was greater than 2 seconds, if so reset the test as it would be invalid
      reset2();
      printNewMessage(error_message2);
      arrayInit2();
    }

    if (counter2 == measure_limit && data_effect2) {
      counter2 = 0;
      sum2();
    } else if (counter2 != measure_limit && data_effect2) {
      counter2++;
    } else {
      counter2 = 0;
      data_effect2 = true;
    }

  }

  else if (!is_started) { // reset the test if the test has been paused as data would not be valid then
    reset2();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//initializes the temp array, which will store time readings for every sensor reading.
void arrayInit1() {
  for (unsigned char i = 0; i < 20; i++) {
    temp1[i] = 0;
  }
  temp1[20] = millis();
}

void arrayInit2() {
  for (unsigned char i = 0; i < 20; i++) {
    temp2[i] = 0;
  }
  temp2[20] = millis();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//calculates the heart rate over 20 readings from the sensor and prints it to the screen

void sum1() {
  if (data_effect1) { // only calculate if the variable showing whether the data is valid is true.
    heart_rate1 = total / (temp1[20] - temp1[0]);
    Serial.println(result_message1+String(heart_rate1)); // print results in serial monitor screen to reduce terminal screen overloading. Results will be shown in UI anyways.
  }
  data_effect1 = true;
}

void sum2() {
  if (data_effect2) { // only calculate if the variable showing whether the data is valid is true.
    heart_rate2 = total / (temp2[20] - temp2[0]); 
    Serial.println(result_message2+String(heart_rate2)); // print results in serial monitor screen to reduce terminal screen overloading. Results will be shown in UI anyways.
  }
  data_effect2 = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void blinkSequence(uint32_t color){// makes it on off then on 2 times, with small delay
for (int j = 0; j < 2; j++) { 
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, color);
    }
    pixels.show(); 
    delay(200); 
    pixels.clear(); 
    pixels.show(); 
    delay(200);
  }
}
void lightUpSequence(uint32_t color) {//lights up LEDS one by one
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(250); // Delay for the build up
  }
  delay(500);
  pixels.clear(); 
}
void fadeInSequence() {//Fading effect only blue
    for (int b = 0; b < 256; b++) {
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, b, b)); // Fade in blue
      }
      pixels.show();
      delay(5); 
    }
}
void upAndDown() { // function for the lights fillup and down 
  for (int i = 0; i < NUMPIXELS; i++) { 
      pixels.setPixelColor(i, pixels.Color(255, 0, 127)); //pink color
      pixels.show();
      delay(25);
    }
    for (int i = 10; i >= 0; i--) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 168)); //brighter color
      pixels.show();
      delay(25);
    }
}
void slowFillUp(uint32_t color) {
  for (int i = 0; i < 3; i++) { 
      pixels.setPixelColor(i, pixels.Color(255, 0, 0)); //pink color
      pixels.show();
      delay(500);
    }
}
void noMatchMode(uint32_t color) {
  color = pixels.Color(255, 0, 0);
  slowFillUp(color); 
  blinkSequence(color);
  delay(150); // The amount of time between each individually lit up pixel  
}

void normalMode(uint32_t color){
  color = pixels.Color(0, 0, 255);
  lightUpSequence(color);
  delay(100);
  blinkSequence(color);
  delay(100);
  fadeInSequence();
}
void crazyMode() { // for ultimate love match
  uint32_t color = pixels.Color(255, 0, 127); // pink color for the blinking
  blinkSequence(color);
  upAndDown();
}


void light(int level) {
  uint32_t color;
  if (level <= 25) {
    for (int i = 0; i < level; i++) {
      noMatchMode(color);
    }
  } else if (level > 25 && level <= 75) {
    for (int i = 0; i < level; i++) {
      normalMode(color);
   }
  } else {
    for (int i = 0; i < level; i++) {
      crazyMode();
   }
  }
}
