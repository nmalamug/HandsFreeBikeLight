#include <Wire.h>
#include <hp_BH1750.h>              //include the library
hp_BH1750 sensor;                   //define the sensor
#define MOSFET D10
#define BUTTON D11

//Assign mosfet and button pins
const int mosfet_pin = 11;
const int button_pin = 7;

int mode = 0;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = 0;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

float lux; // The value of the photoresistor. Used in series with another resistor, voltage divider. 
float writeval;

unsigned long luxdebDelay = 3000;
float lastLux = 0;
unsigned long luxdebTime = 3000;
float luxdb = 0;
float lastlastLux = 0;
int maxlux = 30;

float luxt = 0;

unsigned long light = 0;

int flag = 0;

void check_button(){
  // read the state of the switch into a local variable:
  
  int reading = digitalRead(button_pin);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    //Serial.print(millis()-lastDebounceTime);
    //Serial.print("\n");
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // only toggle the LED if the new button state is HIGH
      if (buttonState == 1) {
        if (mode<2){
          mode = mode+1;

        } else{
          mode = 0;

        }
      }
    }
  }
  
  lastButtonState = reading;
  }

float deb_lux(float luxd){
  //if(luxd<maxlux){
  //  luxdb = luxd;
  //}
  //if(luxd>maxlux && ){
     
  //}
  
  if(luxd<maxlux){
    luxdb = luxd;
    flag = 0;
  } else if(flag == 0){
      light = millis();
      flag = 1;
      //Serial.println(flag);
  } else if(millis() - light > luxdebTime){//lastLux>maxlux && lastlastLux>maxlux && 
      luxdb = luxd;
      flag = 2;
      //Serial.print(flag);
    //Serial.println(luxdb);
  } else{
    luxdb = 10;
  }
  //if(millis()-luxdebTime > luxdebDelay){
  //  lastLux = luxd;
  //  lastlastLux = lastLux;
  //  luxdebTime = millis();
  //  Serial.println("Done");
  //  Serial.println(lastLux);
  //  Serial.println(lastlastLux);
 // }
  return luxdb;
}

void update_light(){
  //Take action on light depending on the mode. 
  if (mode == 0){
    //Auto Modem
    if (sensor.hasValue()) {          // most important function of this library!
      lux = sensor.getLux();   // read the result
      sensor.start();          // only start the next measurement after getting the result 
      //do time consuming calculations here AFTER a new measurement was started.    
      //Serial.println(lux);
    }

    luxt = deb_lux(lux);
    Serial.println(luxt);
    if(luxt>5 && luxt<30){
      writeval = floor(255-(8.5*luxt));
    }else if(lux <= 5){
      writeval = 255;
    }else if (lux >= 30){
      writeval = 0;
    }
    
    if(writeval < 0){
      writeval = 0; //Set written value to never go below 0`  
    }
    
    if(writeval > 255){
      writeval = 255;
    }
    
    analogWrite(mosfet_pin,floor(writeval));
    
  } else if (mode == 1){
    //High
    analogWrite(mosfet_pin,140);
  } else if (mode == 2){
    //Low
    analogWrite(mosfet_pin,200);
  } else{
    mode = 0;
  }
}

void setup() {
  //Setup pins
  pinMode(mosfet_pin,OUTPUT); //MOSFET
  pinMode(button_pin,INPUT); //Button
  pinMode(A0,INPUT); //photoresistor

  //Begin serial
  Serial.begin(9600);
  
  //Light Sensor
  sensor.begin(BH1750_TO_GROUND);            // set address and initialize the sensor
  sensor.calibrateTiming();         // calibrate the timings, about 855ms with a bad chip
  sensor.start();            // start measurement first time in setup  
}

void loop() {
  // put your main code here, to run repeatedly:
  check_button();
  update_light();
}
