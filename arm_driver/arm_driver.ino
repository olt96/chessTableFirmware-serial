#include "ServoEasing.hpp"
#include "digitalWriteFast.h"
#define GRABBER_SERVO_PIN PB7
#define RC_PIN PB8
#define EXTENSION_SERVO_PIN PB9
#define GRABBER_MAX 100
#define GRABBER_MIN 12
#define GRABBER_CLOSED  false
#define GRABBER_OPEN  true 

#define EXTENSION_SERVO_SPEED 150
#define GRABBER_SERVO_SPEED 1000

#define SERVO_LOWER_LIMIT 80
#define SERVO_UPPER_LIMIT 180
#define EXTENDED_ARM_THRESHOLD 100 
#define TOGGLE_STATE_THRESHOLD_US 900000


volatile unsigned long startTime = 0;
volatile unsigned long currentTime = 0;
volatile int pulseWidth = 0;
volatile int oldPulseWidth = 0;
volatile long pulses = 0;
volatile long extendedTime = 0;
volatile bool grabber_state = GRABBER_OPEN ;

bool writtenFlag = false;
ServoEasing extensionServo;
ServoEasing grabberServo;
void setup()
{
  pinMode(RC_PIN, INPUT_PULLUP);
  extensionServo.attach(EXTENSION_SERVO_PIN, SERVO_UPPER_LIMIT);
  grabberServo.attach(GRABBER_SERVO_PIN, GRABBER_MIN);
  extensionServo.write(SERVO_UPPER_LIMIT);
  grabberServo.write(GRABBER_MIN);

  extensionServo.setSpeed(EXTENSION_SERVO_SPEED);
  grabberServo.setSpeed(GRABBER_SERVO_SPEED);

  extensionServo.setEasingType(EASE_LINEAR);
  grabberServo.setEasingType(EASE_QUADRATIC_OUT);
  attachInterrupt(digitalPinToInterrupt(RC_PIN), PulseTimer, CHANGE);
}

void PulseTimer()
{
  currentTime = micros();
  if (digitalRead(RC_PIN) == HIGH)
  {
    pulses = currentTime - startTime;
    startTime = currentTime;
  }
  else
  {
    pulses += currentTime - startTime;
    pulses = ((float)(currentTime - startTime) / pulses) * 100;
    if (pulses > 1)
    {
      pulseWidth = map(pulses, 0, 100, SERVO_LOWER_LIMIT, SERVO_UPPER_LIMIT);
    }
    startTime = currentTime;
  }
}

void loop()
{

  if (oldPulseWidth != pulseWidth)
  {
    extensionServo.easeTo(pulseWidth);
    if (pulseWidth < EXTENDED_ARM_THRESHOLD) 
    {
      if (!writtenFlag){
      grabber_state = !grabber_state ;
      delay(100);
      grabber_state? grabberServo.easeTo(GRABBER_MIN) : grabberServo.easeTo( GRABBER_MAX );
      writtenFlag = true;
      }
    } else {
      writtenFlag = false;
    }
    oldPulseWidth = pulseWidth;
  } 
  delay(100);
}