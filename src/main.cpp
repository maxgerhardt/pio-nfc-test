#include <Arduino.h>
#include <Cardreader.h>

IPSDisplay display;
Cardreader reader(display);

void setup() {
  Serial.begin(115200);
  Serial.println("Firmware entry.");
  reader.begin();
}

void loop() {
  delay(100);
}