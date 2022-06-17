#include "Adafruit_PWMServoDriver.h"

#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

String inputData = "";
uint16_t servo;
uint16_t pulse[16];

/* ------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(9600);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
}

/* ------------------------------------------------------------------------- */

void loop()
{
  while(Serial.available() == 0) {}
  inputData = Serial.readString();

  if (inputData == "ok")
  {
    for (int i=0; i<16; i++)
    {
      Serial.print(pulse[i]); Serial.print(", ");
    }
    Serial.println();
  }
  else
  {
    int blank = inputData.indexOf(" ");
    Serial.println(inputData);
    servo = inputData.substring(0, blank).toInt();
    pulse[servo] = inputData.substring(blank+1).toInt();
    pwm.setPWM(servo, 0, pulse[servo]);
  }

	inputData = "";
}