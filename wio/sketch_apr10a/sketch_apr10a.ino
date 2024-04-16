#include <TFT_eSPI.h>
TFT_eSPI tft;

#define STANDARD_HORIZONTAL_VIEW 3
#define START BUTTON_3
#define NEXT_Q BUTTON_2
#define HELP BUTTON_1

String start_message ="Hello and Welcome to The Love Detector. Press the left button to begin. While in test, press the left button to stop the test or the middle button to change to the next question. Press the right button for help.";
String result_message = "Your heart rate is: ";
String loading_message = "Heart rate test has begun. You will be notified when it is complete or if there is an issue.";
String error_message= "Heart rate measure error, test will restart automatically. Make sure the sensor is attached securely!";
String reset_message= "Test has been stopped. Press button again to reset the test.";


unsigned long temp[21];
unsigned char counter = 0;
bool data_effect = true;
unsigned int heart_rate;
const int max_heartpluse_duty = 2000;
volatile bool pressed_start= false;
bool previous_state=false;


void printMessage(String string)
{
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.println(string);
}

void clear()
{
  tft.fillScreen(TFT_WHITE);
}

void press()
{
  if(digitalRead(START)==LOW)
  {
    pressed_start=!pressed_start;
  }
}

void reset()
{
  data_effect=0;
  counter=0;
}

void setup() {
  pinMode(START,INPUT);
  tft.begin();
  tft.setRotation(STANDARD_HORIZONTAL_VIEW); //Set up commands to display messages on the Wio screen
  Serial.begin(9600);  //Open port to display messages in the Arduino IDE terminal

  printMessage(start_message);


  arrayInit();  //initialize the data array with 0s
  attachInterrupt(digitalPinToInterrupt(START), press, CHANGE);
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
  if(pressed_start!=previous_state)
{
  if(pressed_start)
  {
      clear();
     printMessage(loading_message); 

  }
  else if(!pressed_start)
   {
     clear();
     printMessage(reset_message);
    }
  
 previous_state=pressed_start;
}

}
void interrupt() 
{
  if(pressed_start)
  {
  temp[counter] = millis();

  switch (counter) 
  {
    case 0:
      break;
    default:
      unsigned long sub = temp[counter] - temp[counter - 1];
      if (sub > max_heartpluse_duty) 
      {
        data_effect = false;
        counter = 0;
        clear();
        printMessage(error_message);
        arrayInit();
        return;
      }
      break;
  }

  if (counter == 20 && data_effect) 
  {
    counter = 0;
    sum();
  } 
  else if (counter != 20 && data_effect)
  {
    counter++;
  }
  else 
  {
    counter = 0;
    data_effect = true;
  }

  }

  else if(!pressed_start)
  {
    reset();
  }

}

void arrayInit() {
  for (unsigned char i = 0; i < 20; i++) {
    temp[i] = 0;
  }
  temp[20] = millis();
}

void sum() {
  if (data_effect) 
  {
    heart_rate = 1200000 / (temp[20] - temp[0]);
    clear();
    printMessage(result_message+ String(heart_rate));
  }
  data_effect = true;
}
