#ifndef Cardreader_h
#define Cardreader_h

#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <IPSDisplay.h>

class Cardreader
{
  public:
    Cardreader(IPSDisplay &ipsDisplay);
    void begin();
    static void scanCard(void * parameter);
  private:
    void process();
    TaskHandle_t task_ScanCard;
    IPSDisplay *_ipsDisplay;
    PN532_SPI pn532spi;
    PN532 nfc;
};

#endif