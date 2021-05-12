#include <Arduino.h>
#include <Cardreader.h>

IPSDisplay display;
Cardreader reader(display);

void setup() {
  reader.begin();
}

void loop() {
  delay(100);
}