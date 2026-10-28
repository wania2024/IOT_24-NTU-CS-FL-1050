//Wania Abdul Basit 24-NTU-CS-FL-1050


#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

const int LED1_PIN = 18;
const int LED2_PIN = 19;

const int CHANNEL1 = 0;
const int CHANNEL2 = 2;

const int FREQ1 = 1000;
const int FREQ2 = 5000;
const int RESOLUTION = 8;
const int MAX_DUTY = 255;

int duty1 = 0;
int duty2 = 0;

void showOLED() {
    int percent1 = duty1 * 100 / MAX_DUTY;
    int percent2 = duty2 * 100 / MAX_DUTY;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("PWM MONITOR");

    display.setCursor(0, 18);
    display.print("LED1: ");
    display.print(percent1);
    display.print("% F:");
    display.print(FREQ1);
    display.println("Hz");

    display.setCursor(0, 38);
    display.print("LED2: ");
    display.print(percent2);
    display.print("% F:");
    display.print(FREQ2);
    display.println("Hz");

    display.display();
}

void setup() {
    Serial.begin(115200);

    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("OLED initialization failed!");
        while (true) {
            delay(1000);
        }
    }

    ledcSetup(CHANNEL1, FREQ1, RESOLUTION);
    ledcSetup(CHANNEL2, FREQ2, RESOLUTION);

    ledcAttachPin(LED1_PIN, CHANNEL1);
    ledcAttachPin(LED2_PIN, CHANNEL2);

    ledcWrite(CHANNEL1, 0);
    ledcWrite(CHANNEL2, 0);

    Serial.println("Task 2: Dual LED PWM with OLED");

    showOLED();
}

void loop() {
    for (duty1 = 0; duty1 <= MAX_DUTY; duty1++) {
        duty2 = duty1;

        ledcWrite(CHANNEL1, duty1);
        ledcWrite(CHANNEL2, duty2);

        showOLED();
        delay(10);
    }

    for (duty1 = MAX_DUTY; duty1 >= 0; duty1--) {
        duty2 = duty1;

        ledcWrite(CHANNEL1, duty1);
        ledcWrite(CHANNEL2, duty2);

        showOLED();
        delay(10);
    }
}
