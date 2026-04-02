# ESP32 Environmental Sensor System

## Overview
This project is an embedded environmental sensing system built around the ESP32-C3 Zero.
It was developed as part of undergraduate research and is designed to collect environmental
data using multiple sensors while supporting low-power operation, timing control, and
modular code organization.

## Project Goals
- Collect environmental sensor data reliably
- Support low-power operation using deep sleep
- Maintain accurate timing using an external RTC
- Enable modular and maintainable embedded code
- Prepare the system for wireless communication and data logging

## Features
- Embedded system modular desing
- Power management for battery powered deployments
- Power conservation by using deep sleep
- Time synchronization using external RTC
- Wireless communication using LoRa point-to-point
- Micro-SD data logging

## Hardware Used
- ESP32-C3 Zero
- DS3231 RTC
- AHT21
- BME280
- Seesaw soil sensor
- INA219
- Micro-SD card module
- LoRa module

## Software
- Arduino IDE
- Embedded libraries
- Autodesk Fusion

## Engineering Challenges
- Managing deep sleep wake timing accurately
- Correcting drift between wake cycles and logging intervals
- Configure the system for long term battery powered field use
- Designing a dense PCB within the parameters of a deployable enclosure.

## Documentation
- ESP32 vs Raspberry Pi Pico comparison
- PCB module specifications with datasheet references
- Environmental data log in CSV format

## Repository Structure
- `src/` — source code
- `docs/` — project documentation and notes
- `images/` — project photos and diagrams

## Future Improvements
- Add clearer wiring and system diagrams
- Improve documentation for setup and deployment
- Add power consumption measurements
- Expand wireless communication and remote logging features

## Author
Steven Pilgrim
Computer Engineering Student, NDSU
