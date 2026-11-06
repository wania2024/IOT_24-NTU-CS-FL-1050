#include <Arduino.h>

#define DAC_PIN 25

int delays[] = {5, 20, 50};

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < 3; i++) {
    Serial.print("Fade with delay = ");
    Serial.print(delays[i]);
    Serial.println(" ms");

    for (int value = 0; value <= 255; value++) {   // brighter
      dacWrite(DAC_PIN, value);
      delay(delays[i]);
    }
    for (int value = 255; value >= 0; value--) {   // dimmer
      dacWrite(DAC_PIN, value);
      delay(delays[i]);
    }
  }
}