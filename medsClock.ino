#include <SimpleTimer.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27,16,2);

// https://forum.arduino.cc/t/making-an-arduino-clock/485102
// ideas from here

int currentDay = 0;

//SYSTEM VARIABLES
SimpleTimer timer;
const int ledSwitchPin = 7;
int ledPinArray[] = {8,9}; //,7,8}; //REMEMBER TO CHANGE THE CONSTANT IN THE []s.

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
  lcd.print("Mo's Meds V2");
  lcd.setCursor(0,1);

  rtc.begin();
  
  pinMode(ledSwitchPin, INPUT);
  timerID = timer.setInterval(timerLengthMs, setTimerFlag);
  for (int i = 0; i < numLeds; i++)
    pinMode(ledPinArray[i], OUTPUT);
}

void loop()
{
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
  resetFlag = true;
}

void ledRoutine()
{
    ledSwitchPressed = digitalRead(ledSwitchPin);
    //switch released
    if (!ledSwitchPressed && ledSwitchDebouncer)
    {
      timer.disable(timerID);
      ledSwitchDebouncer = false;
    }
    //switch pressed
    else if (ledSwitchPressed && !ledSwitchDebouncer)
    {
      ledSwitchDebouncer = true;
      digitalWrite(ledPinArray[ledCounter], HIGH);
      incrementCounter();

      //resetTimer
      resetFlag = false; //TODO: check whether this is necessary here
      timerID = timer.setTimeout(timerLengthMs, setTimerFlag);
      timer.restartTimer(timerID);
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
  ledCounter++;
  if (ledCounter == numLeds) //counter rollover.
    ledCounter = 0;
}
