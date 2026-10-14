#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- Pin and display configuration ----------------
const uint8_t  BUTTON_PIN    = 18;     // push button to GND
const uint8_t  SDA_PIN       = 21;     // I2C data
const uint8_t  SCL_PIN       = 22;     // I2C clock
const uint8_t  OLED_ADDRESS  = 0x3C;   // usual SSD1306 I2C address
const uint8_t  SCREEN_WIDTH  = 128;
const uint8_t  SCREEN_HEIGHT = 64;

// ---------------- Debounce configuration ----------------
const unsigned long DEBOUNCE_MS = 50;  // input must be stable this long

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- Debounce state variables ----------------
int           lastRawReading   = HIGH; // last raw (possibly bouncing) level
int           stableState      = HIGH; // last accepted (debounced) level
unsigned long lastChangeTime   = 0;    // when the raw level last changed

// ---------------- Application state ----------------
unsigned long pressCount = 0;          // number of accepted presses

// Redraw the whole status screen
void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32 Button Counter");
  display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print("Button: ");
  display.setTextSize(2);
  display.println(stableState == LOW ? "PRESSED" : "RELEASED");

  display.setTextSize(1);
  display.setCursor(0, 44);
  display.print("Count: ");
  display.setTextSize(2);
  display.println(pressCount);

  display.display();   // push the buffer to the OLED over I2C
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // idle HIGH, pressed LOW

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 init failed - check wiring/address");
    while (true) { delay(1000); }      // halt: no point continuing
  }

  // Start from the real current button level
  lastRawReading = digitalRead(BUTTON_PIN);
  stableState    = lastRawReading;

  updateDisplay();                     // title, status, count = 0
  Serial.println("System ready. Press the button.");
}

void loop() {
  int rawReading = digitalRead(BUTTON_PIN);

  // 1) Raw level changed (could be real or bounce): restart the stability timer
  if (rawReading != lastRawReading) {
    lastChangeTime = millis();
    lastRawReading = rawReading;
  }

  // 2) Level has been stable for DEBOUNCE_MS and differs from accepted state
  if ((millis() - lastChangeTime) >= DEBOUNCE_MS && rawReading != stableState) {
    stableState = rawReading;          // accept the new level

    if (stableState == LOW) {          // valid press (HIGH -> LOW edge)
      pressCount++;
      Serial.print("Accepted press. Count = ");
      Serial.println(pressCount);
    } else {
      Serial.println("Button released.");
    }
    updateDisplay();                   // refresh only when something changed
  }

  // loop() never blocks, so other tasks could run here too.
}