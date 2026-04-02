
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
/// ======================= ///

#ifndef INITIALIZER_H
#define INITIALIZER_H

class Initializer {
  public:

    Initializer(RTCManager &rtcMgr,
                RTC_DS3231 &rtc,
                Adafruit_INA219 &ina,
                Adafruit_AHTX0 &aht,
                Adafruit_seesaw &ssw,
                Adafruit_BME280 &bme);

    void begin(bool &i2cOK,
              bool &rtcOK,
              bool &ahtOK,
              bool &sswOK,
              bool &inaOK,
              bool &bmeOK,
              bool &msdOK);

  private:

    RTCManager &rtcManager;
    RTC_DS3231      &rtc;
    Adafruit_INA219 &ina;
    Adafruit_AHTX0  &aht;
    Adafruit_seesaw &ssw;
    Adafruit_BME280 &bme;

    void systemBootStatus();
    void initializeLoRa();
    void verifyCalibrationAHT21(bool &ahtOK);
    bool checkAHT21Calibration(uint8_t &statusByte);
    void reinitializeAHT21();
    void initializeSDReader(bool &msdOK);
    void verifyInitRunTime();
};

#endif