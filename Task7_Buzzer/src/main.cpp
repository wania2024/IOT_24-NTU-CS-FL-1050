#include <Arduino.h>

#define BUZZER_PIN 16
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 1000
#define PWM_RESOLUTION 8

void setup() {
  // Configure PWM channel
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

  // Attach PWM channel to GPIO 16
  ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
}

void loop() {
  // 500 Hz tone
  ledcWriteTone(PWM_CHANNEL, 500);
  delay(1000);

  // 1000 Hz tone
  ledcWriteTone(PWM_CHANNEL, 1000);
  delay(1000);

  // Stop buzzer
  ledcWriteTone(PWM_CHANNEL, 0);
  delay(1000);
}