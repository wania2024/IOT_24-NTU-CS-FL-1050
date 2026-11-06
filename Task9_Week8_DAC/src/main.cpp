#include <Arduino.h>

#define DAC_PIN 25

void setup() {}

void loop() {
  for (int value = 0; value <= 255; value++) {   // getting brighter
    dacWrite(DAC_PIN, value);
    delay(20);
  }
  for (int value = 255; value >= 0; value--) {   // getting dimmer
    dacWrite(DAC_PIN, value);
    delay(20);
  }
}