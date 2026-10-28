//Wania Abdul Basit 24-NTU-CS-FL-1050
#include <Arduino.h>

constexpr uint8_t LED1_PIN = 18;
constexpr uint8_t LED2_PIN = 19;

constexpr uint8_t PWM_CHANNEL1 = 0;
constexpr uint8_t PWM_CHANNEL2 = 2;

constexpr uint32_t PWM_FREQUENCY1 = 1000;
constexpr uint32_t PWM_FREQUENCY2 = 5000;

constexpr uint8_t PWM_RESOLUTION = 8;
constexpr uint16_t PWM_MAX_DUTY = (1 << PWM_RESOLUTION) - 1;

void setup()
{
    Serial.begin(115200);

    double actualFreq1 = ledcSetup(
        PWM_CHANNEL1, PWM_FREQUENCY1, PWM_RESOLUTION
    );

    double actualFreq2 = ledcSetup(
        PWM_CHANNEL2, PWM_FREQUENCY2, PWM_RESOLUTION
    );

    if (actualFreq1 == 0 || actualFreq2 == 0)
    {
        Serial.println("ERROR: PWM setup failed.");
        while (true)
        {
            delay(1000);
        }
    }

    ledcAttachPin(LED1_PIN, PWM_CHANNEL1);
    ledcAttachPin(LED2_PIN, PWM_CHANNEL2);

    ledcWrite(PWM_CHANNEL1, 0);
    ledcWrite(PWM_CHANNEL2, 0);

    Serial.println("Dual LED PWM Fade Started");

    Serial.print("LED 1 Frequency: ");
    Serial.print(actualFreq1);
    Serial.println(" Hz");

    Serial.print("LED 2 Frequency: ");
    Serial.print(actualFreq2);
    Serial.println(" Hz");
}

void loop()
{
    // Fade in both LEDs
    for (int duty = 0; duty <= PWM_MAX_DUTY; duty++)
    {
        ledcWrite(PWM_CHANNEL1, duty);
        ledcWrite(PWM_CHANNEL2, duty);

        delay(10);
    }

    // Fade out both LEDs
    for (int duty = PWM_MAX_DUTY; duty >= 0; duty--)
    {
        ledcWrite(PWM_CHANNEL1, duty);
        ledcWrite(PWM_CHANNEL2, duty);

        delay(10);
    }
}
