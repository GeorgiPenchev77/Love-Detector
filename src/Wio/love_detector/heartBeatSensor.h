/* --------------------------------------------------------------------------- */
// The solution for the interrupt function and general logic of how heartbeat can
// be measured using these sensors are adapted from:
// https://wiki.seeedstudio.com/Grove-Ear-clip_Heart_Rate_Sensor/
/* --------------------------------------------------------------------------- */

#ifndef HBS_H
#define HBS_H                                                   // header guard

#include "util.h"                                               // include utility functions

/* ------------------------------ Wio Macros --------------------------------- */

#define LEFT 0                                                  // left Sensor
#define RIGHT 1                                                 // right Sensor

#define LEFT_PIN PIN_WIRE_SCL                                   // left port pin
#define RIGHT_PIN 0                                             // right port pin

/* ------------------------------ Variables ---------------------------------- */

extern volatile bool previousState;

class HBSensor{
  public:
    /* 
     * maximum delay between sensor logs in ms
     * i.e. if there are no new logs for >2 seconds it will trigger an error
     */
    static const int MAX_HEARTPULSE_DUTY = 2000;  
    static const int TOTAL = 1200000;                           // const used to calculate heart-rate
    static const int MEASURE_LIMIT = 20;                        // the limit of sensor measurements

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
    volatile bool dataEffect = false;                           // store whether data is valid or not
    volatile bool isUpdated = false;
    byte SENSOR;
    volatile bool isActive = false;

/* --------------------------- Functions ---------------------------------- */

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
     * attachInterrupt(pin, ISR, mode) creates an "event listener", that triggers whenever specified event happens on the monitored pins.
     * In our case:
     *  - "digitalPinToInterrupt(x)" shows that we're monitoring changes on Digital Pin x
     *  - "interrupt[...]" is the name of the callback function, called when a change occurs
     *  - "RISING" specifies that we only need to call the function when the pin goes from LOW to HIGH
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
      /* 
       * interrupt function is stopped via button press,
       * when stopped the test will be marked as invalid and reset to run again,
       * it will start running on next button press 
       */

      if (isActive) {

        temp[counter] = millis();                                   // start counting time between sensor ticks

        if(counter != 0){
          sub = temp[counter] - temp[counter - 1];
        }


        if (sub > MAX_HEARTPULSE_DUTY) {                            // check whether the time between the now and the last tick was greater than 2 seconds, 
          reset();                                                  // if so reset the test as it would be invalid
          if(SENSOR == LEFT){
            updateWioText(ERROR_MESSAGE1);
          } else if (SENSOR == RIGHT){
            updateWioText(ERROR_MESSAGE2);
          }
          return;
        }

        if (counter == MEASURE_LIMIT) {
          counter = 0;
          isUpdated = true;
          sum();
        } else if (counter != MEASURE_LIMIT) {
          counter++;
        }

      }

      else if (!isActive) {                                         // reset the test if the test has been paused as data would not be valid then
        reset();
      }
    }

    void sum() {                                                    // calculates the heart rate over 20 readings from the sensor and prints it to the screen
      heartRate = TOTAL / (temp[(MEASURE_LIMIT+counter) % (MEASURE_LIMIT+1)] - temp[counter]);

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

    /*
     * getIMHeartrate()
     * Returns individaul heartrate messured by the current user, addinally tracking
     * the number of calls made and deactivating sensor when needed
     */
    static int getIMHeartrate(){
      IMCounter++;
      updateWioText(LOADING_MESSAGE);
      Serial.printf("Get IM request. Requests done: %d\n", IMCounter);

      if(isIMStarted && IMCounter >= IMCounterMAX){                 // in IM mode: check if the sufficient number of measures has been reached
        IMCounter = 0;
        deactivateSensor(currentIMUser);
        updateWioText(RESET_MESSAGE);
      }

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
      activateSensor(LEFT);
      activateSensor(RIGHT);
    }

    static void stopDate(){
      isDateStarted = false;
      deactivateSensor(LEFT);
      deactivateSensor(RIGHT);
    }

    static void activateSensor(const byte SENSOR){
      if(instances[SENSOR] -> isActive == true){
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

    static void startIM(int measurementsRequired){                  // turn on IM mode - to be called only on MQTT message  
      IMCounterMAX = measurementsRequired;
      isIMStarted = true;
      Serial.printf("IM started. CounterMax = %d\n", IMCounterMAX);
    }

    static void stopIM(){                                           // turn off IM mode - to be called only on MQTT message
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
      IMCounter = 0;
      Serial.printf("IM user switched to %d.\n", currentIMUser);
    }

    static void processStartClick(){
      if(isIMStarted){
        activateSensor(currentIMUser);                              // activating an active sensor will lead to deactivating that sensor
      }
    }

    static void processStopClick(){
      if(instances[currentIMUser] -> isActive == false){
        return;
      }
      if(isIMStarted){
        deactivateSensor(currentIMUser);
      }
    }

/* -------------------------------------------------------------------------- */

 private:
    unsigned int  heartRate;

    void reset(){
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

/* -------------------------------------------------------------------------- */

#endif                                                             // end header guard