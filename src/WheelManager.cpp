#include "WheelManager.h"
#include <Arduino.h>

//float WheelManager::wheelCircumfranceInches = 84.5;
float WheelManager::wheelCircumfranceInches = 105.6;
float WheelManager::miliageAdjustment = 0;
uint32_t WheelManager::validTickCount = 0;
uint32_t WheelManager::validTicks[MAX_TICK_COUNT];

#if WHEEL_MANAGER_DEBUG == 1
void wmTest()
{
  WheelManager wm;

  wm.wheelCircumfranceInches = 84.5;

  while(true)
  {
    for (int i = 0; i < 25; i++)
    {
      wm.AddTickRaw();
      // 0.06 seconds -> 60ms -> 15hz -> 60mph ish
      unsigned long msDelay = 51; 
      delay(msDelay);
    }

    float speed = wm.GetSpeed(1);
    float distaince = wm.GetTotalDistance();
    Serial.print("WM Speed: " + String(speed));
    Serial.print("WM distaince: " + String(distaince));
    Serial.println("");

    delay(1000);
  }
}
#endif


WheelManager::WheelManager()
{

}

int8_t WheelManager::beging()
{
  // knh todo - get wheelCircumfranceInches from eeprom.
  ///WheelManager::wheelCircumfranceInches = 84.5;
  WheelManager::wheelCircumfranceInches = 105.6;
  WheelManager::validTickCount = 0;
  miliageAdjustment = 0;
  return 0;
}

float WheelManager::GetSpeed(uint8_t durationSeconds)
{ 
  // we have a few cases:
  // 1) the last arival was more than duration away
  //    then speed is zero.
  // 2) there were more than one arrivals with in duration
  //    then speed is arrivalCount * timeSpan

  uint32_t nowMs = millis();

  #if WHEEL_MANAGER_DEBUG == 1
  Serial.println("GS: durationSeconds: " + String(durationSeconds));
  Serial.println("GS: nowMs: " + String(nowMs));
  #endif

  uint16_t numberOfTicksGoneBack = 0;
  uint16_t lastTickIndex = 0; 
  uint32_t lastTickArrivalMs = 0;
  uint32_t msSenseLastTick = 0;

  bool keepGoing = true;
  while (keepGoing)
  {
    lastTickIndex = (validTickCount - 1 - numberOfTicksGoneBack) % MAX_TICK_COUNT;
    lastTickArrivalMs = validTicks[lastTickIndex];
    msSenseLastTick = nowMs - lastTickArrivalMs;

    #if WHEEL_MANAGER_DEBUG == 1
    Serial.println("GS: lastTickIndex " + String(lastTickIndex) 
      + " arrivalMs: " + String(lastTickArrivalMs));
    #endif

    if(msSenseLastTick >= (durationSeconds * 1000))
    {
      #if WHEEL_MANAGER_DEBUG == 1
      Serial.println("GS: found end duration: " + String(msSenseLastTick));
      #endif
      break;
      keepGoing = false;
    }

    numberOfTicksGoneBack++;
    if (numberOfTicksGoneBack == MAX_TICK_COUNT)
    {
      // we have gone all the way around the array
      // get out of here

        #if WHEEL_MANAGER_DEBUG == 1
        Serial.println("GS: checked whole array: " + String(numberOfTicksGoneBack));
        #endif
        keepGoing = false;
        break;
    }

    if(numberOfTicksGoneBack == validTickCount)
    {
      #if WHEEL_MANAGER_DEBUG == 1
      Serial.println("GS: examiled all valid tickes");
      #endif
      break;
    }
  }
  
  float inchesTravled = (float(numberOfTicksGoneBack) * wheelCircumfranceInches);
  #if WHEEL_MANAGER_DEBUG == 1
  Serial.println("inchesTraveled: " + String(inchesTravled));
  Serial.println("");
  #endif
  return inchesTravled / float(durationSeconds) / 17.6;
}

void WheelManager::AddTickRaw()
{  
   // AddTickRaw, check if new tick is valid
   // if valid, tick is added to deque, old ticks are bumpped oute.
   // and vlaidTickCount++;
   // A tick is valid if it arrived 50ms after the last tick
   // at 6ft cirmfurance @ 15hz => 68 mph => 0.066 seconds between ticks.

  uint32_t nowMs = millis();
  uint32_t lastArrivalTimeMs = 0;
  uint16_t lastTickIndex = 0;
  // get last tick, if it exists
  if(validTickCount > 0)
  {
    lastTickIndex = (validTickCount - 1) % MAX_TICK_COUNT;
    lastArrivalTimeMs = validTicks[lastTickIndex];
  }

  uint16_t msFromLastTick = nowMs - lastArrivalTimeMs;

  // Serial.println("wm:atr:validTickCount: " + String(validTickCount));
  // Serial.println("wm::AddTick:LastTickI:" + String(lastTickIndex));
  //Serial.println("wm::AddTick:nowMs:" + String(nowMs));
  //Serial.println("wm::AddTick:MIN_MS_BETWEEN_TICKS:" + String(MIN_MS_BETWEEN_TICKS));
  //Serial.println("wm::AddTick:lastArrivalTimeMs:" + String(lastArrivalTimeMs));
  // Serial.println("wm::AddTick:msFromLastTick:" + String(msFromLastTick));
  // Serial.println("");

  if(msFromLastTick >= MIN_MS_BETWEEN_TICKS)
  {
    uint16_t nextTickIndex = validTickCount % MAX_TICK_COUNT;

    // Serial.println("wm::AddTick:ADDING_TICK");
    // Serial.println("wm::AddTick:nextTickIndex:" + String(nextTickIndex));
    // Serial.println("");

    validTicks[nextTickIndex] = nowMs;
    validTickCount++;
  }
}

float WheelManager::GetTotalDistance()
{
  // 63,360 inches per mile: 100 mi 6,336,000
  double totalInches = double(validTickCount) * double(wheelCircumfranceInches); 
  return float(totalInches / 63360) + miliageAdjustment;
}

// add distance for reset
void WheelManager::AddDistance(float milesAdjustment)
{
  miliageAdjustment += milesAdjustment;
}

// add one tenth mile for each call.
void WheelManager::ChangeDistanceAndCalibrate(float milesAdjustment)
{
  miliageAdjustment += milesAdjustment;
  // knh todo - calibrate wheel size for new distance
}

void WheelManager::SetDistanceToZero()
{
  validTickCount = 0;
}