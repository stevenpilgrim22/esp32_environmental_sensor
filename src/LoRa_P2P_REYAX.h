
#ifndef LORA_P2P_REYAX_H
#define LORA_P2P_REYAX_H

class LoRa_P2P_REYAX {
  public:
    void initializeLoRa();
    void collectFromNetwork();
    void sendCommand(String command);
    void sendTheData(String data);
    bool waitForPermission(String nodeNumber);
    void wakeLoRa();
    void sleepLoRa();

  
  private:
    
}

#endif