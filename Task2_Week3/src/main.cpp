//Week3-Lecture1
//Interrupt (External -Button)
//Embeddes IoT System Fall-2026

//Name:Wania Abdul Basit  Reg#: 24-NTU-CS-FL-1050



#include <Arduino.h>

const int buttonPin = 32;
const int ledPin = 4;
volatile bool ledState = LOW;

void IRAM_ATTR handleButton() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, FALLING);
}

void loop() {
  // main loop free for other tasks
}