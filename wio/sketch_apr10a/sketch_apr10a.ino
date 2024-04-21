#include <TFT_eSPI.h>
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


void printMessage(String string) {
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
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

  printMessage(start_message);

  arrayInit();  //initialize the data array with 0s
  /*
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(0)" shows that we're monitoring changes on Digital Pin 0(D0)
        - "interrupt" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
  */
  attachInterrupt(digitalPinToInterrupt(0), interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(START), press, CHANGE);
}

//the loop will only change if there is a button press
void loop() {
  if (is_started != previous_state) {
    if (is_started) {
      clear();
      printMessage(loading_message);

    } else if (!is_started) {
      clear();
      printMessage(reset_message);
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
        if (sub > max_heartpluse_duty) {
          data_effect = false;
          counter = 0;
          clear();
          printMessage(error_message);
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
    printMessage(result_message + String(heart_rate));
  }
  data_effect = true;
}
