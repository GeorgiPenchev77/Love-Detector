//Interrupt solution inspired by https://arduino.stackexchange.com/questions/73287/interrupts-inside-a-class-attaching-to-the-function-of-the-class

#ifndef HBS_H
#define HBS_H

#include "util.h"

extern volatile bool isStarted;
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
    unsigned int  heartRate;
    unsigned long sub;
    unsigned long temp[MEASURE_LIMIT+1];
    unsigned char counter;     
    volatile bool dataEffect = true;    // Store whether data is valid or not
    volatile bool isUpdated = false;
    byte SENSOR;

    HBSensor(byte whichSENSOR){
      SENSOR = whichSENSOR;
      reset();
    }

    void setIsUpdated(){
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

      if (isStarted) {
        temp[counter] = millis(); //start counting time between sensor ticks

        if(counter != 0){
          sub = temp[counter] - temp[counter - 1];
        }


        if (sub > MAX_HEARTPULSE_DUTY) { //check whether the time between the now and the last tick was greater than 2 seconds, if so reset the test as it would be invalid
          reset();
          printNewMessage(ERROR_MESSAGE);
        }

        if (counter == MEASURE_LIMIT && dataEffect) {
          counter = 0;
          sum();
        } else if (counter != MEASURE_LIMIT && dataEffect) {
          counter++;
        } else {
          counter = 0;
          dataEffect = true;
        }

      }

      else if (!isStarted) { // reset the test if the test has been paused as data would not be valid then
        reset();
      }
    }

    //calculates the heart rate over 20 readings from the sensor and prints it to the screen
    void sum() {
      if (dataEffect) { // only calculate if the variable showing whether the data is valid is true.
        heartRate = TOTAL / (temp[20] - temp[0]);
        // Print results in serial monitor screen to reduce terminal screen overloading. Results will be shown in UI anyways.
        Serial.println(RESULT_MESSAGE+String(heartRate)); 
        isUpdated = true;
      }
      dataEffect = true;
    }

 private:
    void reset(){
      sub = 0;
      counter = 0;
      dataEffect = false;
      for (unsigned char i = 0; i < MEASURE_LIMIT; i++) {
        temp[i] = 0;
      }
      temp[MEASURE_LIMIT-1] = millis();
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

HBSensor leftSensor(LEFT);
HBSensor rightSensor(RIGHT);


#endif