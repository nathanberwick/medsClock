#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include "RTClib.h" //https://github.com/adafruit/RTClib
//#define DEBUG

RTC_DS3231 rtc;
const int rs = 7, en = 3, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// https://forum.arduino.cc/t/making-an-arduino-clock/485102
// ideas from here

int currentDay = 0;

//SYSTEM VARIABLES
SimpleTimer timer;
const int ledSwitchPin = 5;
const int ledPinArray[] = {6, A0, A1, A2, A3};

bool ledSwitchPressed = false;
bool resetFlag = false;
bool ledSwitchDebouncer = false;

int ledCounter = 0;
const int numLeds = 4;
const int timerLengthMs = 2000;
int previousSecond = 0;
int timerID;

void setup()
{
    rtc.begin();
    lcd.begin(16, 2);
  lcd.setCursor(0, 0);
#ifndef DEBUG
  lcd.print("Mo's Meds V2");
  lcd.setCursor(0, 1);
#endif


  pinMode(ledSwitchPin, INPUT);
  timerID = timer.setInterval(timerLengthMs, setTimerFlag);
  //timer.enable(timerID);
  for (int i = 0; i < numLeds; i++)
    pinMode(ledPinArray[i], OUTPUT);


//reupdates to time of compilation
        if (rtc.lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
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
  lcd. setCursor(0, 0);
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
    resetFlag = false;
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
    lcd. setCursor(0, 0);
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
  lcd.setCursor(0, 1);
  if (hour < 10)
    lcd.print(0);
  lcd.print(hour);
  lcd.print(':');

  if (minute < 10)
    lcd.print(0);
  lcd.print(minute);
  lcd.print(':');
 
  if (second < 10)
    lcd.print(0);
  lcd.print(second);
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
