
#include "LoRa_P2P_REYAX.h"
#include "Macros.h"

void LoRa_P2P_REYAX::initializeLoRa() {

  // Initializes serial monitor 1.
  Serial1.begin(115200, SERIAL_8N1, UART_RX, UART_TX);
  uint32_t serial1StartTime = millis();
  while (!Serial1 && millis() - serial1StartTime < 500) { delay(10); }

  // Clears the serial1 buffer.
  while (Serial1.available()) { Serial1.read(); }

  // Tests if the LoRa module is responding to commands.
  Serial.println(F("Testing if the LoRa module is responding..."));
  sendTheCommand("AT");

  // Sets the band frequency of the LoRa module.
  Serial.println(F("Setting the band frequency of the LoRa module..."));
  sendTheCommand("AT+BAND=868500000");

  // Sets the network ID for a group of LoRa modules.
  Serial.println(F("Setting the network ID for the LoRa network..."));
  sendTheCommand("AT+NETWORKID=" + NETWORK_ID);

  // Sets the address for the LoRa module.
  Serial.println(F("Setting the address for the LoRa module..."));
  sendTheCommand("AT+ADDRESS=" + LORA_ADDRESS);

  // Sets the parameters of the LoRa module.
  Serial.println(F("Setting the parameters of the LoRa module..."));
  sendTheCommand("AT+PARAMETER=7,3,4,5");
}

void LoRa_P2P_REYAX::collectFromNetwork() {

  //
  for (int n = 1; n <= 8; n++) {

    //
    Serial.print(F("Giving the talking stick to node "));
    Serial.println(n);
    String message = ",GO" + String(n);
    sendTheCommand("AT+SEND=" + String(n) + "," + String(message.length()) + message);

    //
    bool messageReceived = false;
    uint32_t receiveStartTime = millis();
    uint32_t receiveEndTime;
    do {

      //
      if (Serial1.available()) {

        //
        messageReceived = true;
        Serial.print(F("Time to receive message: "));
        Serial.println(receiveEndTime);

        //
        String message = Serial1.readStringUntil("\n");
        message.trim();

        //
        if (message.length() > 0) {
          appendFile(SD, DATA_LOG, message.c_str());
          Serial.print(F("Received message: "));
          Serial.println(message);
        }
      }

      //
      receiveEndTime = millis() - receiveStartTime;
      delay(10);
    } while (!messageReceived && receiveEndTime < RECEIVE_PERIOD * 1000);
  }
}

void LoRa_P2P_REYAX::sendTheCommand(String command) {
  
  //
  Serial1.println(command);
  delay(100);

  //
  String response;
  while(Serial1.available()) { response += char(Serial1.read()); }
  response.trim();
  Serial.print(F("AT command response: "));
  Serial.println(response);
}

void LoRa_P2P_REYAX::sendTheData(String data) {
  sendTheCommand("AT+SEND=0," + String(data.length()) + "," + data);
}

bool LoRa_P2P_REYAX::waitForPermission(String nodeNumber) {

  //
  bool permission = false;

  //
  uint32_t waitStartTime = millis();
  while (millis() - waitStartTime > RECEIVE_PERIOD * 1000) {

    //
    if (Serial1.available()) {

      //
      String incomingMsg = Serial1.readStringUntil("\n");
      String expectedMsg = "GO" + nodeNumber;

      //
      if (incomingMsg.substr(5,3) == expectedMsg) { permission = true;}
    }
  }

  return permission;
}

void LoRa_P2P_REYAX::sleepLoRa() {

}