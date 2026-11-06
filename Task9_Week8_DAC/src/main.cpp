#include <Arduino.h>

#define DAC_PIN 25
#define PWM_PIN 16
#define PWM_CHANNEL 0

void setup() {
  ledcSetup(PWM_CHANNEL, 5000, 8);      // channel, 5 kHz, 8-bit
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);  // connect pin to channel
}

void loop() {
  dacWrite(DAC_PIN, 128);               // steady voltage
  ledcWrite(PWM_CHANNEL, 128);          // 128/255 duty, about 50%
}