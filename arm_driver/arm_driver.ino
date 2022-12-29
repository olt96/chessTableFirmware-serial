#include "ServoEasing.hpp"
#include "digitalWriteFast.h"
#define RCPin PB8
#define GRABBER_MAX 100
#define GRABBER_MIN 12
volatile unsigned long startTime = 0;
volatile unsigned long currentTime = 0;
volatile int pulseWidth = 0;
volatile int oldPulseWidth = 0;
volatile long  pulses = 0;
volatile long  printPulse; 
volatile long extendedTime = 0;
const int servoLowerLimit = 80;
const int servoUpperLimit = 180;
const int extendedArmThreshold = 70;
volatile int grabberExtensionAngle = GRABBER_MIN;

//long PulseWidth = 0;
bool writtenFlag = false;
ServoEasing extensionServo;
ServoEasing grabberServo;
void setup() {
  Serial.begin(115200);
  pinMode(RCPin, INPUT_PULLUP);
  extensionServo.attach(PB9, servoUpperLimit);
  grabberServo.attach(PB7, grabberExtensionAngle);
   extensionServo.write( servoUpperLimit);
  grabberServo.write( grabberExtensionAngle);
 
  extensionServo.setSpeed(55);
   grabberServo.setSpeed(1000);
  
   extensionServo.setEasingType(EASE_LINEAR);
   grabberServo.setEasingType(EASE_QUADRATIC_OUT);
  attachInterrupt(digitalPinToInterrupt(RCPin),PulseTimer,CHANGE);
}

void PulseTimer(){
  currentTime = micros();
  if(digitalRead(RCPin) == HIGH) {
    pulses = currentTime - startTime;
    startTime = currentTime;
  
  }else {
    pulses += currentTime-startTime;
    extendedTime += pulses;
    pulses = ((float)(currentTime-startTime) / pulses) * 100;
    if ( pulses > 1 ){ 
    pulseWidth = map(pulses,  0 , 100 , servoLowerLimit, servoUpperLimit);
    }
    if (pulses < 25 ) {
        if (extendedTime > 1500000  && !writtenFlag) {
            if (grabberExtensionAngle == GRABBER_MAX) {
                grabberExtensionAngle = GRABBER_MIN;
            }else { 
                grabberExtensionAngle = GRABBER_MAX;
            }
            writtenFlag = true;
        }
      }else {
          extendedTime = 0;
          writtenFlag = false;
      }
      startTime = currentTime;
    }
}
long servoWatchdog = 0;
void loop() {

  if(oldPulseWidth != pulseWidth) {
 extensionServo.startEaseTo(pulseWidth);
 oldPulseWidth = pulseWidth;
    } 
    if(writtenFlag) { 
  grabberServo.write(grabberExtensionAngle);
  
    }

}