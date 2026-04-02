
#include "RTCManager.h"
#include <EEPROM.h>
#include <SPI.h>
#include "Macros.h"

RTCManager::RTCManager(RTC_DS3231 &rtc) : rtc(rtc) {}

// This method initializes the external RTC.
void RTCManager::begin(bool &rtcOK) {

  // Initializes the external RTC.
  rtcOK = rtc.begin();

  // Attempts to reinitialize in case of failure.
  if (!rtcOK) {
    delay(100);
    rtcOK = rtc.begin();
  }
  Serial.println(rtcOK ? F("External RTC initialized.") : F("External RTC initialization failed."));

  // Verifies the functionality of the external RTC.
  if (rtcOK) {

    // Finds the current and elapsed time since the last log.
    timeFinder();

    // Verifies the RTC's time.
    verifyRTCTime();

    // Enables built-in drift compensator of the DS3231.
    enableDriftCompensator(RTC_DRIFT_OFFSET);

    // Disables all unused functionalities of the DS3231 RTC.
    disableDS3231Redundancies();
  } else {

    // Marks the period of initializing the system.
    setInitRunTime();
    Serial.print(F("Initialization run time (ms): "));
    Serial.println(getInitRunTime());

    // Puts the system to sleep due to external RTC initialization failure.
    goToSleep(true);
  }
}

// This method will determine the current and elapsed time since last log.
// This is a sub-method of the initializeRTC method.
void RTCManager::timeFinder() {

  // Determines the current unix time.
  Serial.print(F("Current unix time: "));
  Serial.println(getGlobalTime());

  // Determines if it's the first cycle and updates the elapsed Time.
  if (getCycleEndTime() == 0) {
    Serial.println(F("First cycle or RTC time anomaly detected."));
    setElapsedTime(0);
  } else { setElapsedTime(getGlobalTime() - getCycleEndTime()); }
  Serial.print(F("Elapsed time since last log: "));
  Serial.println(getElapsedTime());

  // Verifies if the log interval has passed or if it's the first cycle.
  if (getCycleEndTime() == 0 || getElapsedTime() >= LOG_INTERVAL) {
    Serial.println(F("Time sync conditions met."));
  } else { Serial.println(F("Time sync conditions not met.")); }
}

// This method verifies the RTC's time.
// This is a sub-method of the initializeRTC method.
void RTCManager::verifyRTCTime() {

  // Determines if the external RTC experienced a loss of power.
  if (rtc.lostPower()) {
    Serial.println(F("Adjusting date and time due to external RTC's loss of power..."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// This method enables the built-in drift compensator of the DS3231.
// This is a sub-method of the initializeRTC method.
void RTCManager::enableDriftCompensator(int8_t offset) {

  // Enables the aging offset register of the DS3231 RTC.
  Wire.beginTransmission(0x68);
  Wire.write(0x10);
  Wire.write(offset);
  Wire.endTransmission();
}

// This method will disable all unused functionalities of the DS3231 RTC.
// This is a sub-method of the initializeRTC method.
void RTCManager::disableDS3231Redundancies() {

  // Disables the alarms, 32kHz output, and INT.
  Wire.beginTransmission(0x68);
  Wire.write(0x0E);
  Wire.write(0b00000000);
  Wire.endTransmission();

  // Disables the alarm flags.
  Wire.beginTransmission(0x68);
  Wire.write(0x0F);
  Wire.write(0b00000000);
  Wire.endTransmission();
}

// This method puts the ESP32 into deep-sleep mode and disables I2C/SPI buses.
void RTCManager::goToSleep(bool initFailure) {

  // Disable I2C/SPI buses for power conservation.
  Wire.end();
  SPI.end();

  // Verifying for successful initialization.
  if (initFailure) {

    // Prevents early sleep despite failure to maintain log time consistency.
    while (getCycleTime() < INIT_BUFFER) { delay(10); }

    // Setting the sleep timer based on the initialization buffer time.
    sleepDuration = LOG_INTERVAL * 1000 - INIT_BUFFER;
  } else {

    // Determines the time taken in the logging period.
    setLogRunTime();
    Serial.print(F("Logging run time (ms): "));
    Serial.println(getLogRunTime());

    // Setting the sleep timer based on the run time.
    if (getCycleEndTime() == 0) {
      sleepDuration = LOG_INTERVAL * 1000 - ((getInitRunTime() + getLogRunTime()));
    } else { sleepDuration = LOG_INTERVAL * 1000 - ((INIT_BUFFER + getLogRunTime())); }
  }

  // Prepare for deep sleep in micro-seconds.
  uint64_t sleep_us = (uint64_t)sleepDuration * 1000ULL;

  // Declares the sleep duration.
  Serial.print(F("Sleeping for (ms): "));
  Serial.println(sleepDuration);
  Serial.print(F("Sleeping for (s): "));
  Serial.println(sleepDuration / 1000.0, 3);

  // Sets the ESP32 deep-sleep alarm clock.
  esp_sleep_enable_timer_wakeup(sleep_us);

  // Determines the cycle end time based on the current cycle time.
  Serial.print(F("Cycle end time (s): "));
  Serial.println(getCycleTime());

  // Flush serial ports before sleep.
  Serial.println(F("Now entering deep sleep..."));
  Serial.flush();
  Serial1.flush();
  Serial1.end();

  // Saves the cycle end time to EEPROM for future reference.
  setCycleEndTime(getCycleTime());

  // The ESP32 is put to sleep.
  esp_deep_sleep_start();
}

// Sets the start up time based on the boot up time recorded immediately upon wake up.
void RTCManager::setStartUpTime(uint32_t bootUpTime) { startUpTime = bootUpTime; }

// Sets the initialization run time based on the current cycle time.
void RTCManager::setInitRunTime() { initRunTime = getCycleTime(); }

// Sets the logging run time based on the current cycle time and initialization buffer period.
void RTCManager::setLogRunTime()  { logRunTime = getCycleTime() - INIT_BUFFER; }

// 
void RTCManager::setElapsedTime(uint32_t eTime) { elapsedTime = eTime; }

// Sets the unix time in EEPROM for future reference of when the last cycle ended.
void RTCManager::setCycleEndTime(uint32_t cycleEndTime) {
  EEPROM.writeULong(EEPROM_ADDR_CYCLE_END_TIME, cycleEndTime);
  EEPROM.commit();
}

// Gets the start up time.
uint32_t RTCManager::getStartUpTime() { return startUpTime; }

// Gets the initialization run time.
uint32_t RTCManager::getInitRunTime() { return initRunTime; }

// Gets the logging run time.
uint32_t RTCManager::getLogRunTime()  { return logRunTime; }

// 
uint32_t RTCManager::getElapsedTime() { return elapsedTime; }

// Gets the last cycle's end time from EEPROM.
uint32_t RTCManager::getCycleEndTime() {
  EEPROM.get(EEPROM_ADDR_CYCLE_END_TIME, cycleEndTime);
  return cycleEndTime;
}

// Gets the current cycle time based on millis in relation to start up time.
uint32_t RTCManager::getCycleTime() { return millis() - getStartUpTime(); }

// Gets the current unix time.
uint32_t RTCManager::getGlobalTime() { return rtc.now().unixtime();
}
