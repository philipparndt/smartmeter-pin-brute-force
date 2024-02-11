#include <Arduino.h>
#include <SPI.h>
#include "intercom.h"
#include <Wire.h>
#include "SSD1306Wire.h"

// HARDWARE CONFIGURATION -------
HardwareSerial customSerial(2);
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL
#define IR_TX 14
#define IR_RX 4
#define IR_TX_NOT_RELEVANT 5
// ------------------------------

// PIN CONFIGURATION ------------
#define PIN_DIRECTION 1
#define START_PIN 0
#define MAX_PIN 9999
#define MIN_PIN 0
// ------------------------------

void serialEvent() {
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        Serial.write(inChar);
    }
}

int pinDirection = PIN_DIRECTION;
int pin = START_PIN - PIN_DIRECTION;
bool pinFound = false;
int referenceMessageLength = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    customSerial.begin(9600, SERIAL_8N1, IR_RX, IR_TX_NOT_RELEVANT); // RX on GPIO 4, TX on GPIO 14

    referenceMessageLength = getMessageLength();

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    pinMode(IR_TX, OUTPUT);
}

void nextPin() {
    pin = pin += pinDirection;

    if (pin > MAX_PIN) {
        pin = MAX_PIN;
        pinDirection = -1;
    } else if (pin < MIN_PIN) {
        pin = MIN_PIN;
        pinDirection = 1;
    }
}

void makePulse() {
    digitalWrite(IR_TX, HIGH);
    delay(100);
    digitalWrite(IR_TX, LOW);
    delay(200);
}

void initPinInput() {
    // flash the LED to start a new number with the display test.
    makePulse();
    // flash the LED to go to PIN input.
    makePulse();
}

void sendPin(int pin) {
    initPinInput();

    char formattedPin[5];
    sprintf(formattedPin, "%04d", pin);

    for (int i = 0; i < 4; i++) {
        int digit = formattedPin[i] - '0';
        for (int j = 0; j < digit; j++) {
            makePulse();
        }
        delay(3100);
    }
}

int getMessageLength() {
    int old = 0;

    // Reset input buffer
    customSerial.flush();
    delay(100);

    // Get new input buffer size
    int new_size = customSerial.available();

    if (new_size > 0) {
        // Wait for complete data messages
        while (old < new_size) {
            delay(100);
            old = new_size;
            new_size = customSerial.available();
        }

        old = 0;
        new_size = 0;
        customSerial.flush();
    }

    // Wait until data is received
    while (old == new_size) {
        delay(100);
        old = new_size;
        new_size = customSerial.available();
    }

    // Wait for complete data messages
    while (old < new_size) {
        delay(100);
        old = new_size;
        new_size = customSerial.available();
    }

    return new_size;
}

void loop() {
    if (pinFound) {
        display.clear();
        display.drawString(0, 0, "Pin found: " + String(pin));
        display.display();
        return;
    }

    nextPin();

    Serial.println("Brute forcing pin: " + String(pin));

    display.clear();
    display.drawString(0, 0, "Brute forcing pin...");

    char formattedPin[5];
    sprintf(formattedPin, "%04d", pin);
    display.drawString(0, 20, "Pin: " + String(formattedPin));
    display.display();

    sendPin(pin);

    int messageLength = getMessageLength();
    if (messageLength > referenceMessageLength) {
        pinFound = true;
    }
}
