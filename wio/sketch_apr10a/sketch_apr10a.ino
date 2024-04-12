#include <TFT_eSPI.h>

#define STANDARD_HORIZONTAL_VIEW 3

TFT_eSPI tft;

unsigned long temp[21];
unsigned char counter = 0;
bool data_effect = true;
unsigned int heart_rate;
const int max_heartpluse_duty = 2000;

void setup() {
  //Set up commands to display messages on the Wio screen
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW);

  arrayInit();  //initialize the data array with 0s

  Serial.begin(9600);  //Open port to display messages in the Arduino IDE terminal
  delay(5000);
  Serial.println("Heart rate test begin.");
  attachInterrupt(digitalPinToInterrupt(0), interrupt, RISING);
  /*
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(0)" shows that we're monitoring changes on Digital Pin 0(D0)
        - "interrupt" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
  */
}

void loop() {
}

void interrupt() {
  temp[counter] = millis();

  switch (counter) {
    case 0:
      break;
    default:
      unsigned long sub = temp[counter] - temp[counter - 1];
      if (sub > max_heartpluse_duty) {
        data_effect = false;
        counter = 0;
        Serial.println("Heart rate measure error, test will restart!");
        arrayInit();
        return;
      }
      break;
  }

  if (counter == 20 && data_effect) {
    counter = 0;
    sum();
  } else if (counter != 20 && data_effect)
    counter++;
  else {
    counter = 0;
    data_effect = true;
  }
}

void arrayInit() {
  for (unsigned char i = 0; i < 20; i++) {
    temp[i] = 0;
  }
  temp[20] = millis();
}

void sum() {
  if (data_effect) {
    heart_rate = 1200000 / (temp[20] - temp[0]);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.print("Heart Rate: ");
    tft.println(heart_rate);
    Serial.println(heart_rate);
    //delay(5000);
  }
  data_effect = true;
}
