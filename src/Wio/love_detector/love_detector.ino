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
#define NEXT_Q BUTTON_2
#define HELP BUTTON_1

//Neopixels
#define PIN            D0
#define NUMPIXELS      10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


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



///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

/*WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);*/

const char broker[] = SECRET_IP;
int        port     = 1883;
const char topic[]  = "test";
const char topic2[]  = "real_unique_topic_2";
const char topic3[]  = "real_unique_topic_3";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;




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

void setup() {



  pinMode(START, INPUT); //initialize the button as an input device
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);  //Set up commands to display messages on the Wio screen


  /*WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  printNewMessage("Attempting to connect to ");
  printNewMessage(ssid);
  

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
     failed, retry
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
  

  printNewMessage(start_message);


  arrayInit();  //initialize the data array with 0s
  
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(0)" shows that we're monitoring changes on Digital Pin 0(D0)
        - "interrupt" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
  
  attachInterrupt(digitalPinToInterrupt(0), interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(START), press, CHANGE);*/

  //Neopixels Setup
  randomSeed(analogRead(0));
  pixels.setBrightness(255);
    pixels.begin(); // This initializes the NeoPixel library.

    // Set all pixels to "off" (black)
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
     pixels.show();
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

  //Neopixels test-value
  int randomLevel = random(1, 100);
  light(80);
  delay(200);
}

/*interrupt function is stopped via button press,
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
  }
  data_effect = true;
}

void alternatingBlink(uint32_t color){
 for (int j = 0; j < 10 ; j++){
  pixels.setPixelColor(i, color);
  
 }
}

void blinkSequence(uint32_t color){
for (int j = 0; j < 5; j++) { // makes it on off then on 5 times, with small delay
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, color); //picks the color from light()
    }
    pixels.show(); 
    delay(200); 
    pixels.clear(); 
    pixels.show(); 
    delay(200); 
  }
}
void lightUpSequence(uint32_t color) {
  
  for (int i = 0; i < NUMPIXELS; i++) {//light up LEDS one by one
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(250); // Delay for the build up
  }
  delay(500);
  pixels.clear(); 
}


void fadeInSequence() {
 //Fading effect
    for (int b = 0; b < 256; b++) {
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(b-1, 0, b/2)); // Fade in
      }
      pixels.show();
      delay(5); 
    }

    delay(1000); 

    for (int j = 0; j < 5; j++) { // 5 blink cycles
      pixels.clear(); 
      pixels.show(); 
      delay(200); 
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 0, 127));
      }
      pixels.show(); 
      delay(200); 
    }

    delay(500); 
}



void light(int level) {
  uint32_t color;
  if (level <= 25) {
    for (int i = 0; i < 3; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // red color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(1500); // The amount of time between each individually lit up pixel
    }
  } else if (level > 25 && level <= 75) {
    color = pixels.Color(0, 0, 255);
    lightUpSequence(color);
    delay(100);
    blinkSequence(color);
    delay(100);
  } else {
    for (int i = 0; i < level; i++) {
      color = pixels.Color(0, 100, 127);
      fadeInSequence(color);
      delay(100);
      color = pixels.Color(255, 0, 0);
      blinkSequence(color);
      delay(100);
      color = pixels.Color(0, 0, 255);
      lightUpSequence(color);
      delay(100);
      
   }
  }
}
