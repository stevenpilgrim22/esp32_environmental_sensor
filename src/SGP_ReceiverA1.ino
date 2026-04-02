
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
#include "Logger.h"
/// ======================= ///

RTC_DS3231      rtc;
Adafruit_INA219 ina;
Adafruit_AHTX0  aht;
Adafruit_seesaw ssw;
Adafruit_BME280 bme;

RTCManager rtcManager(rtc);

void setup() {

  // Sets the ESP32-C3 Zero's processing frequency.
  setCpuFrequencyMhz(80);

  // Initialization status objects.
  bool i2cOK = false, rtcOK = false, ahtOK = false, sswOK = false, inaOK = false, bmeOK = false, msdOK = false;

  // Initializes all.
  Initializer init(rtcManager, rtc, ina, aht, ssw, bme);
  init.begin(i2cOK, rtcOK, ahtOK, sswOK, inaOK, bmeOK, msdOK);


  //
  Logger log(rtcManager, rtc, ina, aht, ssw, bme);
  log.begin(ahtOK, sswOK, inaOK, bmeOK);
}

void loop() {
  // Never runs due to deep sleep.
}
