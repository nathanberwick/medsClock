#include <SimpleTimer.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#define DEBUG

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27,16,2);

// https://forum.arduino.cc/t/making-an-arduino-clock/485102
// ideas from here

int currentDay = 0;

//SYSTEM VARIABLES
SimpleTimer timer;
const int ledSwitchPin = 7;
int ledPinArray[] = {2,4}; //,7,8}; //REMEMBER TO CHANGE THE CONSTANT IN THE []s.

bool ledSwitchPressed = false;
bool resetFlag = false;
bool ledSwitchDebouncer = false;

int ledCounter = 0;
const int numLeds = 2; //3;
const int timerLengthMs = 2000;
int previousSecond = 0;
int timerID;

void setup()
{
  lcd.init();
  lcd.backlight();

 #ifndef DEBUG
  lcd.print("Mo's Meds V2");
  lcd.setCursor(0,1);
 #endif

  rtc.begin();
  
  pinMode(ledSwitchPin, INPUT);
  timerID = timer.setInterval(timerLengthMs, setTimerFlag);
        timer.enable(timerID);
  for (int i = 0; i < numLeds; i++)
    pinMode(ledPinArray[i], OUTPUT);
}

void loop()
{
  timer.run();
  //button read ins
  ledRoutine();

  DateTime now = rtc.now();
  //if time changed, update LCD.
  if (previousSecond != now.second())
  {
    previousSecond = now.second();
    updateLCD(now.hour(), now.minute(), now.second());

    //if new day, reset LEDs.
    if (currentDay != now.day())
    {
      reset();
      currentDay = now.day();
    }
  }

  //reset LEDs if timer exceeded
  if (resetFlag)
    reset();
}

void setTimerFlag()
{
    #ifdef DEBUG
  lcd. setCursor(0,0);
  lcd.print("RESET FUNC HIT ");
  #endif
  resetFlag = true;
}

void ledRoutine()
{
    ledSwitchPressed = digitalRead(ledSwitchPin);
    //switch released
    if (!ledSwitchPressed && ledSwitchDebouncer)
    {
      ledSwitchDebouncer = false;
      timer.disable(timerID);
    }
    //switch pressed
    else if (ledSwitchPressed && !ledSwitchDebouncer)
    {
      ledSwitchDebouncer = true;
      digitalWrite(ledPinArray[ledCounter], HIGH);
      incrementCounter();

      //resetTimer
        #ifdef DEBUG
  lcd. setCursor(0,0);
  lcd.print("START RESET: ");
  #endif
      resetFlag = false; //TODO: check whether this is necessary here
      timer.enable(timerID);
    }
}

void reset()
{
    ledCounter = 0;
    resetFlag = false;
    timer.disable(timerID);
    for (int i = 0; i < numLeds; i++)
      digitalWrite(ledPinArray[i], LOW);
}

void updateLCD(int hour, int minute, int second)
{

  lcd.setCursor(0,1);
  int temp = hour;
  if (temp < 10)
    lcd.print(0, DEC);
  lcd.print(temp, DEC);
  lcd.print(':');
  
  temp = minute;
  if (temp < 10)
    lcd.print(0, DEC);
  lcd.print(temp, DEC);
    lcd.print(':');
  temp = second;
  if (temp < 10)
    lcd.print(0, DEC);
  lcd.print(temp, DEC);
}

void incrementCounter()
{
#ifdef DEBUG
digitalWrite(13, HIGH);
delay(10);
digitalWrite(13, LOW);
#endif
  ledCounter++;
  if (ledCounter >= numLeds) //counter rollover.
    ledCounter = 0;
}
