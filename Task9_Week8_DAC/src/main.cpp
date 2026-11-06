#include <Arduino.h>

#define DAC_PIN 26   // DAC2

void setup() {}

void loop() {
  for (int value = 0; value <= 255; value++) {
    dacWrite(DAC_PIN, value);
    delay(20);
  }
  for (int value = 255; value >= 0; value--) {
    dacWrite(DAC_PIN, value);
    delay(20);
  }
}