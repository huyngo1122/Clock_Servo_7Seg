#include "Adafruit_PWMServoDriver.h"

#define SERVO_FREQ 50
#define SEG_B 1
#define SEG_F 5
#define SEG_G 6

// Test digit 1
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
uint16_t segmentOn[7] = {310, 310, 310, 300, 290, 305, 295};
uint16_t segmentOff[7] = {105, 110, 485, 470, 100, 490, 470};

// Test digit 2
// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// uint16_t segmentOn[7] = {295, 285, 295, 300, 300, 300, 310};
// uint16_t segmentOff[7] = {110, 110, 490, 490, 100, 490, 480};

// Test digit 3
// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// uint16_t segmentOn[7] = {290, 300, 310, 290, 280, 310, 290};
// uint16_t segmentOff[7] = {125, 140, 475, 470, 100, 490, 475};

// Test digit 4
// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// uint16_t segmentOn[7] = {315, 300, 315, 315, 285, 315, 285};
// uint16_t segmentOff[7] = {100, 105, 470, 460, 120, 470, 490};

/* digits[number][segment]
** number : 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
** segment: A(0), B(1), C(2), D(3), E(4), F(5), G(6)
*/
bool digits[10][7] = {    //          A - B - C - D - E - F - G
  {1, 1, 1, 1, 1, 1, 0},  // Digit 0: .   .   .   .   .   .
  {0, 1, 1, 0, 0, 0, 0},  // Digit 1:     .   .
  {1, 1, 0, 1, 1, 0, 1},  // Digit 2: .   .       .   .       .
  {1, 1, 1, 1, 0, 0, 1},  // Digit 3: .   .   .   .           .
  {0, 1, 1, 0, 0, 1, 1},  // Digit 4:     .   .           .   .
  {1, 0, 1, 1, 0, 1, 1},  // Digit 5: .       .   .       .   .
  {1, 0, 1, 1, 1, 1, 1},  // Digit 6: .       .   .   .   .   .
  {1, 1, 1, 0, 0, 0, 0},  // Digit 7: .   .   .
  {1, 1, 1, 1, 1, 1, 1},  // Digit 8: .   .   .   .   .   .   .
  {1, 1, 1, 1, 0, 1, 1}   // Digit 9: .   .   .   .       .   .
};                        // Position values for each digit -> (1) mean have that segment!

byte prevNumber = 8;

uint16_t midOffset = 100;

char incomingByte = 0;

/* ------------------------------------------------------------------------- */

void display(byte number)
{
  /* Move adjacent segments */
  if (digits[number][SEG_G] != digits[prevNumber][SEG_G])
  {
    if (digits[prevNumber][SEG_B])  pwm.setPWM(SEG_B, 0, segmentOn[SEG_B] - midOffset);
    if (digits[prevNumber][SEG_G])  pwm.setPWM(SEG_F, 0, segmentOn[SEG_F] + midOffset);
  }
  delay(100);
  /* Move middle segment if required */
  if (digits[number][SEG_G])  pwm.setPWM(SEG_G, 0, segmentOn[SEG_G]);
  else                        pwm.setPWM(SEG_G, 0, segmentOff[SEG_G]);

  /* Move the remaining segments from A(0) to F(5) */
  for (byte i=0; i<6; i++)
  {
    if (digits[number][i])  pwm.setPWM(i, 0, segmentOn[i]);
    else                    pwm.setPWM(i, 0, segmentOff[i]);
    delay(10);
  }
}

/* ------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(9600);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);

  /* Set all of the servos to ON or up (88:88 displayed) */
  for (int i=0; i<7; i++)
  {
    pwm.setPWM(i, 0, segmentOff[i]);
    delay(100);
  }
  for (int i=6; i>=0; i--)
  {
    pwm.setPWM(i, 0, segmentOn[i]);
    delay(100);
  }
  delay(2000);
}

/* ------------------------------------------------------------------------- */

void loop()
{
  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();

    Serial.print("I received: ");
    Serial.println(incomingByte);

    switch(incomingByte)
    {
      case '0': display(0); prevNumber = 0; break;
      case '1': display(1); prevNumber = 1; break;
      case '2': display(2); prevNumber = 2; break;
      case '3': display(3); prevNumber = 3; break;
      case '4': display(4); prevNumber = 4; break;
      case '5': display(5); prevNumber = 5; break;
      case '6': display(6); prevNumber = 6; break;
      case '7': display(7); prevNumber = 7; break;
      case '8': display(8); prevNumber = 8; break;
      case '9': display(9); prevNumber = 9; 
    }
  }
}