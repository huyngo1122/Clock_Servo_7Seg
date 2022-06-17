/* ------------------------------------------------------------------------- */
/*                                ADD LIBRARY                                */
/* ------------------------------------------------------------------------- */

#include "RTClib.h"
#include "Adafruit_PWMServoDriver.h"

/* ------------------------------------------------------------------------- */
/*                                DEFINE MACRO                               */
/* ------------------------------------------------------------------------- */

/* Analog servos run at ~50 Hz updates */
#define SERVO_FREQ 50

/* All segments of "Hour" (servo position) */
#define HU_A 0  // Hour Units
#define HU_B 1
#define HU_C 2
#define HU_D 3
#define HU_E 4
#define HU_F 5
#define HU_G 6
#define HT_A 9  // Hour Tens
#define HT_B 10
#define HT_C 11
#define HT_D 12
#define HT_E 13
#define HT_F 14
#define HT_G 15

/* All segments of "Minute" (servo position) */
#define MU_A 0  // Minute Units
#define MU_B 1
#define MU_C 2
#define MU_D 3
#define MU_E 4
#define MU_F 5
#define MU_G 6
#define MT_A 9  // Minute Tens
#define MT_B 10
#define MT_C 11
#define MT_D 12
#define MT_E 13
#define MT_F 14
#define MT_G 15

/* Segments */
#define SEG_A 0
#define SEG_B 1
#define SEG_C 2
#define SEG_D 3
#define SEG_E 4
#define SEG_F 5
#define SEG_G 6

/* Buzzer */
#define BUZZ 9

/* ------------------------------------------------------------------------- */
/*                               DECLARE OBJECT                              */
/* ------------------------------------------------------------------------- */

RTC_DS3231 rtc;

Adafruit_PWMServoDriver pwmH = Adafruit_PWMServoDriver(0x40); // Create an object of "Hour" driver
Adafruit_PWMServoDriver pwmM = Adafruit_PWMServoDriver(0x41); // Create an object of "Minute" driver (A0 Address Jumper)

/* ------------------------------------------------------------------------- */
/*                              DECLARE VARIABLE                             */
/* ------------------------------------------------------------------------- */

// ! Cần căn chỉnh cho từng Servo ở hai trạng thái ON và OFF
// 0x40 - ON
// 290, 300, 310, 290, 280, 310, 290, 0, 0, 315, 300, 315, 315, 285, 315, 285 
// 0x40 - OFF
// 125, 140, 475, 470, 100, 490, 475, 0, 0, 100, 105, 470, 460, 120, 470, 490 
//
// 0x41 - ON
// 310, 310, 310, 300, 290, 305, 295, 0, 0, 295, 285, 295, 300, 300, 300, 310 
// 0x41 - OFF
// 105, 110, 485, 470, 100, 490, 470, 0, 0, 110, 110, 490, 490, 95, 490, 480

uint16_t segmentHOn[16] = {           //               A -  B -  C -  D -  E -  F -  G
  290, 300, 310, 290, 280, 310, 290,  // Hour Units:  00 - 01 - 02 - 03 - 04 - 05 - 06
  0, 0,
  315, 300, 315, 315, 285, 315, 285   // Hour Tens:   07 - 08 - 09 - 10 - 11 - 12 - 13
};                                    // ON positions for each "Hour servo"

uint16_t segmentMOn[16] = {           //                 A -  B -  C -  D -  E -  F -  G
  310, 310, 310, 300, 290, 305, 295,  // Minute Units:  00 - 01 - 02 - 03 - 04 - 05 - 06
  0, 0,
  295, 285, 295, 300, 300, 300, 310   // Minute Tens:   07 - 08 - 09 - 10 - 11 - 12 - 13
};                                    // ON positions for each "Minute servo"

uint16_t segmentHOff[16] = {          //               A -  B -  C -  D -  E -  F -  G
  125, 140, 475, 470, 100, 490, 475,  // Hour Units:  00 - 01 - 02 - 03 - 04 - 05 - 06
  0, 0,
  100, 105, 470, 460, 120, 470, 490   // Hour Tens:   07 - 08 - 09 - 10 - 11 - 12 - 13
};                                    // OFF positions for each "Hour servo"

uint16_t segmentMOff[16] = {          //                 A -  B -  C -  D -  E -  F -  G
  105, 110, 485, 470, 100, 490, 470,  // Minute Units:  00 - 01 - 02 - 03 - 04 - 05 - 06
  0, 0,
  110, 110, 490, 490, 95, 490, 480    // Minute Tens:   07 - 08 - 09 - 10 - 11 - 12 - 13
};                                    // OFF positions for each "Minute servo"

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

/* Create variables to store each 7 segment display numeral */
byte hourTens = 0;
byte hourUnits = 0;
byte minuteTens = 0;
byte minuteUnits = 0;

/* Create variables to store the previous numeral displayed on each
** This is required to move the segments adjacent to the middle ones out of the way when they move
*/
byte prevHourTens = 8;
byte prevHourUnits = 8;
byte prevMinuteTens = 8;
byte prevMinuteUnits = 8;

/* Amount by which adjacent segments to the middle move away when required */
uint16_t midOffset = 100;

/* ------------------------------------------------------------------------- */
/*                              DECLARE FUNCTION                             */
/* ------------------------------------------------------------------------- */

/* ----------------------- Update the displayed time ----------------------- */
void updateDisplay()
{
  updateMid();

  /* Move the remaining segments from A(0) to F(5) */
  for (byte i=0; i<6; i++)
  {
    if (digits[hourTens][i])  pwmH.setPWM(i+9, 0, segmentHOn[i+9]);
    else                      pwmH.setPWM(i+9, 0, segmentHOff[i+9]);
    delay(10);

    if (digits[hourUnits][i]) pwmH.setPWM(i, 0, segmentHOn[i]);
    else                      pwmH.setPWM(i, 0, segmentHOff[i]);
    delay(10);

    if (digits[minuteTens][i])  pwmM.setPWM(i+9, 0, segmentMOn[i+9]);
    else                        pwmM.setPWM(i+9, 0, segmentMOff[i+9]);
    delay(10);

    if (digits[minuteUnits][i]) pwmM.setPWM(i, 0, segmentMOn[i]);
    else                        pwmM.setPWM(i, 0, segmentMOff[i]);
    delay(10);
  }
}

/* ------- Move the middle segements and adjacent ones out of the way ------ */
void updateMid()
{
  // ! Giữa số cũ và số mới, đoạn "G" có thay đổi ko?
  // ! Nếu có thì ...
  // ! Nếu ko, về cơ bản các chuyển động của những đoạn còn lại ko ảnh hưởng đến nhau

  /* Move adjacent segments for Minute units */
  if (digits[minuteUnits][SEG_G] != digits[prevMinuteUnits][SEG_G])
  {
    if (digits[prevMinuteUnits][SEG_B]) pwmM.setPWM(MU_B, 0, segmentMOn[MU_B] - midOffset);
    if (digits[prevMinuteUnits][SEG_G]) pwmM.setPWM(MU_F, 0, segmentMOn[MU_F] + midOffset);
  }
  delay(100);
  /* Move Minute units middle segment if required */
  if (digits[minuteUnits][SEG_G]) pwmM.setPWM(MU_G, 0, segmentMOn[MU_G]);
  else                            pwmM.setPWM(MU_G, 0, segmentMOff[MU_G]);

  /* ----------------------------------------------------------------------- */

  /* Move adjacent segments for Minute tens */
  if (digits[minuteTens][SEG_G] != digits[prevMinuteTens][SEG_G])
  {
    if (digits[prevMinuteTens][SEG_B])  pwmM.setPWM(MT_B, 0, segmentMOn[MT_B] - midOffset);
    if (digits[prevMinuteTens][SEG_G])  pwmM.setPWM(MT_F, 0, segmentMOn[MT_F] + midOffset);
  }
  delay(100);
  /* Move Minute tens middle segment if required */
  if (digits[minuteTens][SEG_G])  pwmM.setPWM(MT_G, 0, segmentMOn[MT_G]);
  else                            pwmM.setPWM(MT_G, 0, segmentMOff[MT_G]);

  /* ----------------------------------------------------------------------- */
  /* Move adjacent segments for Hour units */
  if (digits[hourUnits][SEG_G] != digits[prevHourUnits][SEG_G])
  {
    if (digits[prevHourUnits][SEG_B]) pwmH.setPWM(HU_B, 0, segmentHOn[HU_B] - midOffset);
    if (digits[prevHourUnits][SEG_G]) pwmH.setPWM(HU_F, 0, segmentHOn[HU_F] + midOffset);
  }
  delay(100);
  /* Move Hour units middle segment if required */
  if (digits[hourUnits][SEG_G]) pwmH.setPWM(HU_G, 0, segmentHOn[HU_G]);
  else                          pwmH.setPWM(HU_G, 0, segmentHOff[HU_G]);

  /* ----------------------------------------------------------------------- */

  /* Move adjacent segments for Hour tens */
  if (digits[hourTens][SEG_G] != digits[prevHourTens][SEG_G])
  {
    if (digits[prevHourTens][SEG_B])  pwmH.setPWM(HT_B, 0, segmentHOn[HT_B] - midOffset);
    if (digits[prevHourTens][SEG_G])  pwmH.setPWM(HT_F, 0, segmentHOn[HT_F] + midOffset);
  }
  delay(100);
  /* Move Hour tens middle segment if required */
  if (digits[hourTens][SEG_G])  pwmH.setPWM(HT_G, 0, segmentHOn[HT_G]);
  else                          pwmH.setPWM(HT_G, 0, segmentHOff[HT_G]);
}

/* ------------------------------- Tick tick ------------------------------- */
void tick()
{
  digitalWrite(BUZZ, HIGH);
  delay(50);
  digitalWrite(BUZZ, LOW);
  delay(50);
  digitalWrite(BUZZ, HIGH);
  delay(50);
  digitalWrite(BUZZ, LOW);
}

/* ------------------------------------------------------------------------- */
/*                                  SETTING                                  */
/* ------------------------------------------------------------------------- */

void setup()
{
  // Serial.begin(9600);
  // ! Thêm tính năng set time qua serial monitor

  rtc.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(BUZZ, OUTPUT);
  digitalWrite(BUZZ, LOW);

  pwmH.begin();
  pwmM.begin();
  pwmH.setOscillatorFrequency(27000000);
  pwmM.setOscillatorFrequency(27000000);
  pwmH.setPWMFreq(SERVO_FREQ);
  pwmM.setPWMFreq(SERVO_FREQ);

  /* Set all of the servos to ON or up (88:88 displayed) */
  for (int i=0; i<16; i++)
  {
    delay(100);
    pwmH.setPWM(i, 0, segmentHOff[i]);
    delay(100);
    pwmM.setPWM(i, 0, segmentMOff[i]);
  }
  for (int i=15; i>=0; i--)
  {
    delay(100);
    pwmH.setPWM(i, 0, segmentHOn[i]);
    delay(100);
    pwmM.setPWM(i, 0, segmentMOn[i]);
  }
  delay(2000);
}

/* ------------------------------------------------------------------------- */
/*                                  RUN !!!                                  */
/* ------------------------------------------------------------------------- */

void loop()
{
  /* If minute units has changed, update display */
  if (rtc.now().minute() != prevMinuteUnits)
  {
    DateTime now = rtc.now();

    // ! thêm tính năng báo còi theo số giờ đồng hồ, số còi tick tick tương ứng

    /* Split "Hours" into two digits, tens and units */
    hourTens = now.hour() / 10;
    hourUnits = now.hour() % 10;

    /* Split "Minutes" into two digits, tens and units */
    minuteTens = now.minute() / 10;
    minuteUnits = now.minute() % 10;
    if (now.minute() == 0) {tick();} // Tick tick!

    updateDisplay();

    /* Update previous displayed numerals */
    prevHourTens = hourTens;
    prevHourUnits = hourUnits;
    prevMinuteTens = minuteTens;
    prevMinuteUnits = minuteUnits;
  }
  delay(500);

  // for (byte number=0; number<=9; number++)
  // {
  //   hourTens = number;
  //   hourUnits = number;
  //   minuteTens = number;
  //   minuteUnits = number;

  //   updateDisplay();

  //   prevHourTens = hourTens;
  //   prevHourUnits = hourUnits;
  //   prevMinuteTens = minuteTens;
  //   prevMinuteUnits = minuteUnits;

  //   delay(2000);
  // }
  // delay(100);
}
