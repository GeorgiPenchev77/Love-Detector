//Interrupt solution inspired by https://arduino.stackexchange.com/questions/73287/interrupts-inside-a-class-attaching-to-the-function-of-the-class

#ifndef HBS_H
#define HBS_H

#include "util.h"

extern volatile bool previousState;


//Sensors macross
#define LEFT 0
#define RIGHT 1

//Port pins macros
#define LEFT_PIN PIN_WIRE_SCL       // Left port pin
#define RIGHT_PIN 0                 // Right port pin


class HBSensor{
  public:
    // maximum delay between sensor logs in ms
    // i.e. if there are no new logs for >2 seconds it will trigger an error
    static const int MAX_HEARTPULSE_DUTY = 2000;  
    static const int TOTAL = 1200000;    // const used to calculate heart-rate
    static const int MEASURE_LIMIT = 20; //the limit of sensor measurements

    static volatile bool isDateStarted;
    static volatile bool isIMStarted;
    static volatile int currentIMUser;
    static volatile int IMCounter;
    static volatile int IMCounterMAX;
    static volatile int interval;
    static int timer;


    unsigned long sub;
    unsigned long temp[MEASURE_LIMIT+1];
    unsigned char counter;     
    volatile bool dataEffect = false;    // Store whether data is valid or not
    volatile bool isUpdated = false;
    byte SENSOR;
    volatile bool isActive = false;



    HBSensor(byte whichSENSOR){
      SENSOR = whichSENSOR;
      reset();
    }

    void resetIsUpdated(){
      isUpdated=false;
    }

    void setup(){
      switch(SENSOR){
        case LEFT:
          pinMode(LEFT_PIN, INPUT);
          break;
        case RIGHT:
          pinMode(RIGHT_PIN, INPUT);
          break;
      }
      attachSensorInterrupt(SENSOR);
    }
    
    /*
      attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
      In our case:
        - "digitalPinToInterrupt(x)" shows that we're monitoring changes on Digital Pin x
        - "interrupt[...]" is the name of the callback function, called when a change occurs
        - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
    */
    void attachSensorInterrupt(const byte SENSOR, const byte MODE = RISING){
      switch(SENSOR){
        case LEFT: 
          attachInterrupt(digitalPinToInterrupt(LEFT_PIN), interruptLeft, MODE);
          instances[LEFT] = this;
          break;
        case RIGHT:
          attachInterrupt(digitalPinToInterrupt(RIGHT_PIN), interruptRight, MODE);
          instances[RIGHT] = this;
          break;
      }
    }

    void interrupt() {
      /* interrupt function is stopped via button press,
      when stopped the test will be marked as invalid and reset to run again,
      it will start running on next button press */

      if (isActive) {

        //In IM mode: check if the sufficient number of measures has been reached
        if(isIMStarted && IMCounter >= IMCounterMAX){
          deactivateSensor(SENSOR);
        }

        temp[counter] = millis(); //start counting time between sensor ticks

        if(counter != 0){
          sub = temp[counter] - temp[counter - 1];
        }


        if (sub > MAX_HEARTPULSE_DUTY) { //check whether the time between the now and the last tick was greater than 2 seconds, if so reset the test as it would be invalid
          reset();
          if(SENSOR == LEFT){
            printNewMessage(ERROR_MESSAGE1);
          } else if (SENSOR == RIGHT){
            printNewMessage(ERROR_MESSAGE2);
          }
            
        }

        if (counter == MEASURE_LIMIT) {
          counter = 0;
          isUpdated = true;
          sum();
        } else if (counter != MEASURE_LIMIT) {
          counter++;
        }

      }

      else if (!isActive) { // reset the test if the test has been paused as data would not be valid then
        reset();
      }
    }

    //calculates the heart rate over 20 readings from the sensor and prints it to the screen
    void sum() {
      heartRate = TOTAL / (temp[(MEASURE_LIMIT+counter) % (MEASURE_LIMIT+1)] - temp[counter]);
      // Print results in serial monitor screen to reduce terminal screen overloading. Results will be shown in UI anyways.

      switch(SENSOR){
        case LEFT:
          Serial.println(RESULT_MESSAGE1+String(heartRate));
          break;
        case RIGHT:
          Serial.println(RESULT_MESSAGE2+String(heartRate));
          break;
      }

      dataEffect = true;
    }

    static int getIMHeartrate(){
      IMCounter++;
      instances[currentIMUser] -> resetIsUpdated();
      return  instances[currentIMUser] -> heartRate;
    }

    static bool isIMReady(){
      return instances[currentIMUser] -> isUpdated;
    }

    int getCurrentHeartrate(){
      if(dataEffect){
        sum();
      }
      return heartRate;
    }

    static void startDate(int newInterval){
      interval = newInterval;
      isDateStarted = true;
      timer = millis();
    }

    static void stopDate(){
      isDateStarted = false;
      instances[LEFT]->reset();
      instances[RIGHT]->reset();
    }

    static void activateSensor(const byte SENSOR){
      if(instances[SENSOR] -> isActive == true){
        deactivateSensor(SENSOR);
        return;
      }
      instances[SENSOR] -> reset();
      instances[SENSOR] -> isActive = true;
      Serial.printf("Sensor %x activated.\n", SENSOR);
    }

    static void deactivateSensor(const byte SENSOR){

      instances[SENSOR] -> isActive = false;
      instances[SENSOR] -> reset();
      Serial.printf("Sensor %x deactivated.\n", SENSOR);
    }

    //turn on IM mode
    //to be called only on MQTT message
    static void startIM(int measurementsRequired){
      IMCounterMAX = measurementsRequired;
      IMCounter = 0;
      isIMStarted = true;
      Serial.println("IM started.");
    }

    //turn off IM mode
    //to be called only on MQTT message
    static void stopIM(){
      isIMStarted = false;
      Serial.println("IM stopped.");
    }

    static void switchIMUser(){
      switch(currentIMUser){
        case LEFT:
          currentIMUser = RIGHT;
          break;
        case RIGHT:
          currentIMUser = LEFT;
          break;
      }
      Serial.println("IM user switched.");
    }

    static void processStartClick(){
      if(isIMStarted){
        activateSensor(currentIMUser);
        //activating an active sensor will lead to deactivating that sensor
      }
    }

 private:
    unsigned int  heartRate;

    void reset(){
      IMCounter = 0;
      sub = 0;
      counter = 0;
      dataEffect = false;
      for (unsigned char i = 0; i < MEASURE_LIMIT; i++) {
        temp[i] = 0;
      }
    } 
    

    static HBSensor* instances [2];

    static void interruptLeft(){
      if(HBSensor::instances[LEFT] != NULL){
        HBSensor::instances[LEFT]->interrupt();
      }
    }

    static void interruptRight(){
      if(HBSensor::instances[RIGHT] != NULL){
        HBSensor::instances[RIGHT]->interrupt();
      }
    }

};

HBSensor * HBSensor::instances [2] = { NULL, NULL };

volatile bool HBSensor::isDateStarted = false;
volatile int HBSensor::interval = -1;
int HBSensor::timer = 0;

volatile bool HBSensor::isIMStarted = false;
volatile int HBSensor::currentIMUser = LEFT;

volatile int HBSensor::IMCounterMAX = NULL;
volatile int HBSensor::IMCounter = NULL;

HBSensor leftSensor(LEFT);
HBSensor rightSensor(RIGHT);


#endif