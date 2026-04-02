
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include "Macros.h"

#ifndef RTCMANAGER_H
#define RTCMANAGER_H

class RTCManager {
  public:
    RTCManager(RTC_DS3231 &rtc);
    void begin(bool &rtcOK);
    void goToSleep(bool initFailure = false);
    void timeAdjuster();
    void setStartUpTime(uint32_t bootUpTime);
    void setInitRunTime();
    void setLogRunTime();
    void setElapsedTime(uint32_t eTime);
    void setCycleEndTime(uint32_t cycleEndTime);
    uint32_t getStartUpTime();
    uint32_t getInitRunTime();
    uint32_t getLogRunTime();
    uint32_t getElapsedTime();
    uint32_t getCycleEndTime();
    uint32_t getCycleTime();
    uint32_t getGlobalTime();

  private:
    void enableDriftCompensator(int8_t offset);
    void verifyRTCTime();
    void timeFinder();
    void disableDS3231Redundancies();

    RTC_DS3231 &rtc;
    uint32_t startUpTime;
    uint32_t initRunTime;
    uint32_t logRunTime;
    uint32_t elapsedTime;
    uint32_t cycleEndTime;
    uint32_t sleepDuration;
};

#endif