#include <Arduino.h>

constexpr uint8_t  LED_PIN        = 2;      // Built-in LED on most ESP32 dev boards
constexpr uint8_t  PWM_CHANNEL    = 0;      // 0-15 on ESP32
constexpr uint32_t PWM_FREQUENCY  = 5000;
constexpr uint8_t  PWM_RESOLUTION = 8;
constexpr uint16_t PWM_MAX_DUTY   = (1 << PWM_RESOLUTION) - 1;   // 255

void setup()
{
    Serial.begin(115200);

    // Configure the PWM channel (returns the actual frequency, or 0 on failure)
    double actualFreq = ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

    if (actualFreq == 0)
    {
        Serial.println("ERROR: PWM setup failed.");
        while (true) { delay(1000); }
    }

    // Connect the GPIO pin to the PWM channel
    ledcAttachPin(LED_PIN, PWM_CHANNEL);

    ledcWrite(PWM_CHANNEL, 0);
    Serial.println("ESP32 PWM LED Fade Started");
}

void loop()
{
    // Fade in
    for (int duty = 0; duty <= PWM_MAX_DUTY; duty++)
    {
        ledcWrite(PWM_CHANNEL, duty);
        delay(10);
    }

    // Fade out
    for (int duty = PWM_MAX_DUTY; duty >= 0; duty--)
    {
        ledcWrite(PWM_CHANNEL, duty);
        delay(10);
    }
}