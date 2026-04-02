
/// RECEIVER NODE LIBRARIES ///
#include <Wire.h>
#include <WiFi.h>
#include <esp_bt.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_AHTX0.h"
#include "Adafruit_seesaw.h"
#include <Adafruit_INA219.h>
#include <EEPROM.h>
#include <RTClib.h>
#include "esp_sleep.h"
#include "RTCManager.h"
#include "Initializer.h"
/// ======================= ///

#ifndef LOGGER_H
#define LOGGER_H

class Logger {
  public:

    Logger(RTCManager &rtcMgr,
           RTC_DS3231 &rtc,
           Adafruit_INA219 &ina,
           Adafruit_AHTX0 &aht,
           Adafruit_seesaw &ssw,
           Adafruit_BME280 &bme);

    void begin(bool &ahtOK,
               bool &sswOK,
               bool &inaOK,
               bool &bmeOK);
  private:

    RTCManager &rtcManager;
    RTC_DS3231      &rtc;
    Adafruit_INA219 &ina;
    Adafruit_AHTX0  &aht;
    Adafruit_seesaw &ssw;
    Adafruit_BME280 &bme;

    unsigned long computeAllowableDrift(unsigned int logInterval);
    void giveGoSignal();
    void logSensorData(bool &ahtOK, bool &bmeOK, bool &sswOK, bool &inaOK);
    void sleepAHT21();
    String formattedTime(DateTime now);
    void appendFile(fs::FS &fs, const char *path, const char *message);
    void logLoRaMessages();
    void timeAdjuster();
};

#endif