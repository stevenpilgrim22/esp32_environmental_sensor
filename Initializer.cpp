
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

extern String formattedTime(DateTime);
extern void appendFile(fs::FS &fs, const char *path, const char *message);

//
Initializer::Initializer(RTCManager &rtcMgr, RTC_DS3231 &rtc, Adafruit_INA219 &ina, Adafruit_AHTX0 &aht,
                         Adafruit_seesaw &ssw, Adafruit_BME280 &bme) : rtcManager(rtcMgr),
                                                                       rtc(rtc),
                                                                       ina(ina),
                                                                       aht(aht),
                                                                       ssw(ssw),
                                                                       bme(bme) {}

// This method initializes all necessary components of the script.
void Initializer::begin(bool &i2cOK, bool &rtcOK, bool &ahtOK, bool &sswOK,
                        bool &inaOK, bool &bmeOK, bool &msdOK) {
  
  //
  uint32_t bootUpTime = millis();

  // Disables the ESP32-C3 Zero's WiFi.
  WiFi.mode(WIFI_OFF);

  // Disables the ESP32-C3 Zero's Bluetooth.
  btStop();

  // Initializes the I2C bus early to record total boot time.
  i2cOK = Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);

  // Initializes EEPROM memory.
  EEPROM.begin(EEPROM_SIZE);

  // Initializes the serial monitor.
  Serial.begin(115200);
  uint32_t serial0StartTime = millis();
  while (!Serial && millis() - serial0StartTime < 1000) { delay(10); }
  Serial.println(F("\n/// START OF NEW CYCLE ///"));
  Serial.println(F("Waking up..."));

  // Determines the boot up status of the system.
  systemBootStatus();

  // Verifies the initialization of the I2C bus.
  Serial.println(i2cOK ? F("I2C bus initialized.") : F("I2C bus initialization failed."));

  // Initializes and configures the external RTC before finding the time since last log.
  rtcManager.begin(rtcOK);

  // Initializes the LoRa Module and/or wakes it up.
  initializeLoRa();

  // Attempts to initialize and verifies the initialization/calibration of the AHT21.
  uint32_t ahtStartTime = millis();
  while (!(ahtOK = aht.begin()) && millis() - ahtStartTime < 500) { delay(10); }
  verifyCalibrationAHT21(ahtOK);
  Serial.println(ahtOK ? F("AHT21 initialized.") : F("AHT21 initialization failed."));

  // Attempts to and verifies initialization of the STEMMA Seesaw.
  uint32_t sswStartTime = millis();
  while (!(sswOK = ssw.begin(0x36)) && millis() - sswStartTime < 500) { delay(10); }
  Serial.println(sswOK ? F("STEMMA Seesaw initialized.") : F("STEMMA Seesaw initialization failed."));

  // Attempts to and verifies initialization of the INA219.
  uint32_t inaStartTime = millis();
  while (!(inaOK = ina.begin()) && millis() - inaStartTime < 500) { delay(10); }
  Serial.println(inaOK ? F("INA219 initialized.") : F("INA219 initialization failed."));

  // Attempts to and verifies initialization of the BME280.
  uint32_t bmeStartTime = millis();
  while (!(bmeOK = bme.begin(0x77)) && millis() - bmeStartTime < 500) { delay(10); }
  Serial.println(bmeOK ? F("BME280 initialized.") : F("BME280 initialization failed."));

  // Puts the BME280 to sleep unless called on.
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::FILTER_OFF);
  
  // Initializes the micro-SD card reader and data log file.
  initializeSDReader(msdOK);

  //
  verifyInitRunTime();
}

// This method determines the boot up status of the system.
// This is a sub-method of the initializer method.
void Initializer::systemBootStatus() {
  
  // Determines what caused the wake-up.
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  Serial.print("Wakeup cause: ");
  Serial.println((int)cause);

  // Determines the reason for a reset.
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  Serial.println((int)reason);

  // Determines if it is the first cycle.
  if (cause == 0 && reason == 1) { rtcManager.setCycleEndTime(0); }
}

// This method initializes the LoRa module.
// This is a sub-method of the initializer method.
void Initializer::initializeLoRa() {

  // Flushes Serial1 input.
  while (Serial1.available()) {
    Serial1.read();
    delay(1);
  }

  // Initializes and/or wakes the LoRa module.
  Serial1.println(F("AT+SLEEP=0"));
  unsigned long initLoRaTime = millis();
  while (!Serial1.available() && millis() - initLoRaTime < 1000);

  // Verifies that the LoRa module is functioning.
  if (Serial1.available()) {
    String response = Serial1.readStringUntil('\n');
    response.trim();
    if (response.length() > 0) { Serial.println("LoRa Response: " + response); }
  }

  // Sets an address for the node and an ID for the node group.
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
    Serial1.print(F("AT+ADDRESS="));
    Serial1.println(String(LORA_ADDRESS));
    unsigned long setupStartLoRa = millis();
    while (!Serial1.available() && millis() - setupStartLoRa < 200) { delay(10); }
    while (Serial1.available()) { Serial1.read(); }
    Serial1.print(F("AT+NETWORKID="));
    Serial1.println(String(NETWORK_GROUP));
    delay(100);
  }
}

// This method verifies the calibration status of the AHT21.
// This is a sub-method of the initializer method.
void Initializer::verifyCalibrationAHT21(bool &ahtOK) {

  // Determines calibration status to determine proper action.
  uint8_t clbrStatus = 0;
  if (!checkAHT21Calibration(clbrStatus)) {
    Serial.println(F("AHT21 needs to be recalibrated..."));
    reinitializeAHT21();
    if (!checkAHT21Calibration(clbrStatus)) {
      Serial.println(F("AHT21 recalibration failed."));
      Serial.print(F("AHT21 Status Byte: 0x"));
      Serial.println(clbrStatus, HEX);
      ahtOK = false;
    } else {
      Serial.println(F("AHT21 recalibrated."));
    }
  } else {
    Serial.println(F("AHT21 calibrated correctly."));
  }
}

// This method determines the calibration status of the AHT21.
// This is a sub-method of the verifyCalibrationAHT21 method.
bool Initializer::checkAHT21Calibration(uint8_t &statusByte) {

  // Sends a status confirmation command.
  Wire.beginTransmission(0x38);
  Wire.write(0x71);
  Wire.endTransmission();

  // Looks for the proper calibration signal.
  Wire.requestFrom(0x38, 1);
  if (Wire.available()) {
    statusByte = Wire.read();
    return (statusByte & 0x18) == 0x18;
  }
  
  // Indicates that the proper calibration signal was not found.
  statusByte = 0x00;
  return false;
}

// This method reinitializes the AHT21 if not correctly calibrated.
// This is a sub-method of the verifyCalibrationAHT21 method.
void Initializer::reinitializeAHT21() {

  // Initializes register B.
  Wire.beginTransmission(0x38);
  Wire.write(0x1B);
  Wire.endTransmission();
  delay(10);

  // Initializes register C.
  Wire.beginTransmission(0x38);
  Wire.write(0x1C);
  Wire.endTransmission();
  delay(10);

  // Initializes register E.
  Wire.beginTransmission(0x38);
  Wire.write(0x1E);
  Wire.endTransmission();
  delay(10);
}

// This method initializes the micro-SD card reader and data log file.
// This is a sub-method of the initializer method.
void Initializer::initializeSDReader(bool &msdOK) {

  // Initialization counters for micro-SD card reader.
  const uint8_t msdMaxAttempts = 3;
  uint8_t msdAttemptCounter = 0;

  // Initializes the SPI bus.
  SPI.begin(SPI_SCK, SPI_SDI, SPI_SDO);
  SPI.setFrequency(4000000);

  // Initialization loop for micro-SD card reader.
  while (msdAttemptCounter < msdMaxAttempts && !(msdOK = SD.begin(SPI_CS))) {
    msdAttemptCounter++;
    Serial.println(F("Attempting to reinitialize micro-SD reader..."));
    delay(200);
  }
  Serial.println(msdOK ? F("Micro-SD reader initialized.") : F("Micro-SD reader initialization failed."));

  // Verifies the functionality of the micro-SD card reader.
  if (!msdOK) {

    // Marks the period of initializing the system.
    rtcManager.setInitRunTime();
    Serial.print(F("Initialization run time (ms): "));
    Serial.println(rtcManager.getInitRunTime());

    // Puts the system to sleep due to micro-SD card reader initialization failure.
    rtcManager.goToSleep(true);
  }

  // Creates the data log file with a header if it does not yet exist.
  if (!SD.exists(DATA_LOG)) {
    File dataFile = SD.open(DATA_LOG, FILE_WRITE);
    if (dataFile) {
      dataFile.println(CSV_HEADER);
      dataFile.flush();
      dataFile.close();
      Serial.println(F("Header written to data log file."));
    } else { Serial.println(F("Data file creation failed.")); }
  }
}

// This method prevents logging until the right time to maintain log time consistency. 
void Initializer::verifyInitRunTime() {

  // Marks the period of initializing the system.
  rtcManager.setInitRunTime();
  Serial.print(F("Initialization run time (ms): "));
  Serial.println(rtcManager.getInitRunTime());

  // Prevents logging until the initialization buffer period has passed.
  Serial.println(F("Waiting until INIT_BUFFER period has passed..."));
  while (rtcManager.getCycleTime() < INIT_BUFFER) { delay(10); }
}