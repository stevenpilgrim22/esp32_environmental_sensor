
#ifndef MACROS_H
#define MACROS_H

/// CONFIGURABLE INTERVAL ///
#define LOG_INTERVAL 60    // Data log interval in seconds.
#define INIT_BUFFER 2000   // Time buffer for initialization period in milliseconds.
#define SLEEP_BUFFER 0.05  // Seconds before next log time to wake.
/// ===================== ///

/// CONFIGURABLE RTC DRIFT MANAGEMENT ///
#define RTC_DRIFT_OFFSET 0             // Drift calibration variable. 
/// ================================= ///

/// CONFIGURABLE FLASH MEMORY MANAGEMENT ///
#define EEPROM_ADDR_CYCLE_END_TIME 0      // EEPROM address for cycleEndTime
#define EEPROM_SIZE 64                    // Size to store a 64-bit time_t (9 bytes)
/// ==================================== ///

/// NAMES AND PIN ASSIGNMENTS ///
#define I2C_SDA 4              // GPIO 4: I2C data
#define I2C_SCL 5              // GPIO 5: I2C clock

#define SPI_CS  6              // GPIO 6: Chip Select
#define SPI_SCK 7              // GPIO 7: Shared Clock
#define SPI_SDI 8              // GPIO 8: Shared Data In (User in, Module out)
#define SPI_SDO 9              // GPIO 9: Shared Data Out (User out, Module In)

#define UART_RX 20             // GPIO 20: ESP32 RX -> LoRa TX
#define UART_TX 21             // GPIO 21: ESP32 TX -> LoRa RX
/// ========================= ///

/// DATA LOG OBJECTS ///
#define DATA_LOG "/datalog.csv"
#define CSV_HEADER "DEVICE,DATE,TIME,TEMP(C),HUMIDITY(%),PRESSURE(hPa),SOIL TEMP(C),VOLTAGE(V),CURRENT(mA)"
/// ================ ///

///
#define DEVICE_NAME    "A1"     // receiver node name
#define LORA_ADDRESS   "0"      // LoRa address
#define NETWORK_ID     "0"      // Group ID: 0 = A, 1 = B, etc
#define RECEIVE_PERIOD 30
///

#endif