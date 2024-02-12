#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Wire.h"

// HARDWARE CONFIGURATION -------
HardwareSerial customSerial(2);
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL
#define IR_TX 14
#define IR_RX 4
#define IR_TX_NOT_RELEVANT 5
// ------------------------------

#include "readMessage.h"


// PIN CONFIGURATION ------------
#define PIN_DIRECTION -1
#define START_PIN 9999
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
int lastMessageLength = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(IR_TX, OUTPUT);
    digitalWrite(IR_TX, LOW);

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    display.clear();
    display.drawString(0, 0, "Starting...");
    display.display();

    customSerial.begin(9600, SERIAL_8N1, IR_RX, IR_TX_NOT_RELEVANT); // RX on GPIO 4, TX on GPIO 14

    referenceMessageLength = getMaximumMessageLength();
    lastMessageLength = referenceMessageLength;

    display.clear();
    display.drawString(0, 0, "Ref length: " + String(referenceMessageLength));
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, "Wait for initial input");
    display.display();

    delay(1000 + 3100 * 4);
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
    delay(250);
    digitalWrite(IR_TX, LOW);
    delay(250);
}

void initPinInput() {
    delay(800);

    // flash the LED to start a new number with the display test.
    makePulse();
    // flash the LED to go to PIN input.
    // makePulse();

    delay(4000);
}

void sendPin(int pin) {
    display.clear();
    display.drawString(0, 0, "Start PIN input...");
    display.drawString(0, 20, "Pin: " + String(pin));
    display.display();

    initPinInput();

    char formattedPin[5];
    sprintf(formattedPin, "%04d", pin);


    for (int i = 0; i < 4; i++) {
        int digit = formattedPin[i] - '0';

        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0, "Brute forcing pin...");
        display.drawString(0, 20, "Pin: " + String(formattedPin));
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 50, "Sending digit #" + String(i + 1) + "/4 (" + String(digit) + ")");
        display.display();

        for (int j = 0; j < digit; j++) {
            makePulse();
        }
        delay(3100);
    }
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
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Brute forcing pin...");

    char formattedPin[5];
    sprintf(formattedPin, "%04d", pin);
    display.drawString(0, 20, "Pin: " + String(formattedPin));
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 50, String(referenceMessageLength) + " -> " + String(lastMessageLength));
    display.display();

    sendPin(pin);

    int messageLength = getMaximumMessageLength();
    lastMessageLength = messageLength;
    if (messageLength > referenceMessageLength) {
        pinFound = true;
    }
}
