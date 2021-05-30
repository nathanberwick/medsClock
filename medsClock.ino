#include <SimpleTimer.h>



// https://forum.arduino.cc/t/making-an-arduino-clock/485102
// ideas from here


//SYSTEM VARIABLES
SimpleTimer timer;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int ledPinArray[] = {6}; //,7,8}; //REMEMBER TO CHANGE THE CONSTANT IN THE []s.
const int ledSwitchPin = 5;
bool ledSwitchPressed = false;
bool hourSwitchPressed = false;
bool minuteSwitchPressed = false;
bool resetFlag = false;

bool ledSwitchDebouncer = false;
bool hourSwitchDebouncer = false;
bool minuteSwitchDebouncer = false;
const int backlight = 1;
const int minuteSwitchPin = 2;
const int hourSwitchPin = 3;


const int numLeds = 1; //3;

int ledCounter = 0;

int hours = 0;      //Hour integer
int minutes = 0;        //Minute integer
int seconds = 0;        //Seconds integer

const int loopCount = 500; //temp. debug var. use for appropriate button response.
const int resetTimerMs = 2000;
int timerID;

void setup()
{
  pinMode(backlight, OUTPUT);
  pinMode(minuteSwitchPin, INPUT);
  pinMode(hourSwitchPin, INPUT);
  pinMode(ledSwitchPin, INPUT);
  timerID = timer.setInterval(resetTimerMs, setTimerFlag);
  for (int i = 0; i < numLeds; i++)
    pinMode(ledPinArray[i], OUTPUT);
  analogWrite(BackLight, ValBL);
}
void setTimerFlag()
{
  resetFlag = true;
}
void loop()
{
  seconds++;
  Seconds();
  Minutes();
  Hours();

  updateLCD();
  for (int i = 0; i < loopCount; i++)
  {
    ledRoutine();
    hourRoutine();
    minuteRoutine();
    resetRoutine();
    delay(1000/loopCount);
  }
}

void resetRoutine()
{
  if(resetFlag) //timer exceeded
  {
    reset();
    resetFlag = false;
  }
}

void ledRoutine()
{
    ledSwitchPressed = digitalRead(ledSwitchPin);
    if (!ledSwitchPressed && ledSwitchDebouncer)
    {
      timer.disable(timerID);
      ledSwitchDebouncer == false;
    }
    else if (ledSwitchPressed && !ledSwitchDebouncer)
    {
      ledSwitchDebouncer = true;
      digitalWrite(ledPinArray[ledCounter], HIGH);
      incrementCounter();

      //resetTimer
      timer.restartTimer(timerID);
      //resetFlag = false; //TODO: check whether this is necessary here
      timerID = timer.setTimeout(resetTimerMs, setTimerFlag);
    }
}

void hourRoutine()
{
    hourSwitchPressed = digitalRead(hourSwitchPin);
    if (!hourSwitchPressed && hourSwitchDebouncer)
    {
      hourSwitchDebouncer == false;
    }
    else if (hourSwitchPressed && !hourSwitchDebouncer)
    {
      hourSwitchDebouncer = true;
      hours++;
      updateLCD();
    }
}

void minuteRoutine()
{
    minuteSwitchPressed = digitalRead(minuteSwitchPin);
    if (!minuteSwitchPressed && minuteSwitchDebouncer)
    {
      minuteSwitchDebouncer == false;
    }
    else if (minuteSwitchPressed && !minuteSwitchDebouncer)
    {
      minuteSwitchDebouncer = true;
      minutes++;
      updateLCD();
    }
}

void incrementCounter()
{
  ledCounter++;
  if (ledCounter == numLeds) //counter rollover.
    ledCounter = 0;
}

//Loop that adds the Seconds
void Seconds()
{
  if (seconds == 60)
  {
    seconds = 0;
    minutes++;
  }
}

//Loop that adds the Minutes
void Minutes()
{
  if (minutes == 60)
  {
    minutes = 0;
    hours++;
  }
}

//Loop that resets the Hours
void Hours()
{
  if (hours == 24)
  {
    hours = 0;
    reset();
  }
}

void reset()
{
    ledCounter = 0;
    for (int i = 0; i < numLeds; i++)
    {
      digitalWrite(ledPinArray[i], LOW);
    }
}

void updateLCD()
{
  lcd.clear();
  if (Hour < 10)
  {
    lcd.setCursor(2, 1);
    lcd.print(0);
    lcd.print(Hour);
  }
  else
  {
    lcd.setCursor(2, 1);
    lcd.print(Hour);
  }

  lcd.print(" : ");

  if (Min < 10)
  {
    lcd.print(0);
    lcd.print(Min);
  }
  else
  {
    lcd.print(Min);
  }
  
  lcd.print(" : ");

    if (Sec < 10)
  {
    lcd.print(0);
    lcd.print(Sec);
  }
  else
  {
    lcd.print(Sec);
  }
}
