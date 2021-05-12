#include <Arduino.h>
#include <Cardreader.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <IPSDisplay.h>

/* constructor initializes member variables with appropriate constructor arguments */
Cardreader::Cardreader(IPSDisplay &ipsDisplay) : pn532spi(SPI, 5), nfc(pn532spi)
{
    _ipsDisplay = &ipsDisplay;
}

void Cardreader::begin()
{

    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
        Serial.print("Didn't find PN53x board");
        _ipsDisplay->show("/i003");
        delay(5000);
        begin();
    }
    else
    {
        // Got ok data, print it out!
        Serial.print("Found chip PN5");
        Serial.println((versiondata >> 24) & 0xFF, HEX);
        Serial.print("Firmware ver. ");
        Serial.print((versiondata >> 16) & 0xFF, DEC);
        Serial.print('.');
        Serial.println((versiondata >> 8) & 0xFF, DEC);

        // configure board to read RFID tags
        nfc.SAMConfig();

        Serial.println("Waiting for an ISO14443A Card ...");
        xTaskCreatePinnedToCore(this->scanCard, "scanCard", 5000, this, 1, &task_ScanCard, 0);
    }
}

void Cardreader::process()
{
    uint8_t success;
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
    uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    Serial.println("Calling into nfs.readPassiveTargetID()...");
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    Serial.println("...done.");

    if (success)
    {
        // Display some basic information about the card
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");
        Serial.print(uidLength, DEC);
        Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        Serial.println("");

        if (uidLength == 7)
        {
            // We probably have a Mifare Ultralight card ...
            Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

            // Try to read the first general-purpose user page (#4)
            Serial.println("Reading page 4");
            uint8_t data[32];
            success = nfc.mifareultralight_ReadPage(4, data);
            if (success)
            {
                // Data seems to have been read ... spit it out
                nfc.PrintHexChar(data, 4);
                Serial.println("");

                // Wait a bit before reading the card again
                delay(1000);
            }
            else
            {
                Serial.println("Ooops ... unable to read the requested page!?");
            }
        }
    }
    Serial.println("Waiting a second..");
    delay(1000);
}

void Cardreader::scanCard(void *parameter)
{
    Serial.println("Cardreader::scanCard() started with parameter 0x" + String((uint32_t)parameter, HEX));

    //recover concrete Cardreader object given to us
    Cardreader* pCardreader = (Cardreader*) parameter;
    //little error check..
    if(pCardreader == nullptr) { 
        Serial.println("Internal error: scanCard() has received nullptr as parameter..");
        vTaskDelete(nullptr); //delete ourselves
    }
    for (;;)
    {
        //call internal process function where the object has access to all of its member variables
        pCardreader->process();
    }
}