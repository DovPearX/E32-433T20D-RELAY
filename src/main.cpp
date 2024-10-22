#include "Arduino.h"
#include "radio.h"

#define PIN_BUTTON 10
#define PIN_LED_INDICATOR 8
#define RELAY_PIN 3
#define TIMEOUT 3000
#define DEBOUNCE_DELAY 50
#define SEND_DELAY 1000

bool relayState = HIGH;
unsigned long lastCommandTime = 0;
unsigned long lastButtonPressTime = 0;
unsigned long lastSendTime = 0;

void relay_on() {
    if (relayState == HIGH) {
        digitalWrite(RELAY_PIN, LOW);
        relayState = LOW;
        printf("(%lu ms) Relay ON\n", lastCommandTime);
    }

    lastCommandTime = millis();
}

void relay_off() {
    if (relayState == LOW) {
        digitalWrite(RELAY_PIN, HIGH);
        relayState = HIGH;
        printf("(%lu ms) Relay OFF\n", millis());
    }
}

void buttonTask(void *arg) {
    for (;;) {
        bool buttonState = digitalRead(PIN_BUTTON);
        unsigned long currentTime = millis();

        if (buttonState == LOW && (currentTime - lastButtonPressTime > DEBOUNCE_DELAY)) {
            relay_on();

            if (currentTime - lastSendTime > SEND_DELAY) {
                e32ttl100.sendMessage("RELAY_ON\n");
                lastSendTime = currentTime;
            }
            lastButtonPressTime = currentTime;
        } else if (buttonState == HIGH) {
            lastButtonPressTime = currentTime;
        }

        vTaskDelay(10);
    }
}

void receiveTask(void *arg) {
    for (;;) {
        if (e32ttl100.available()) {
            ResponseContainer rc = e32ttl100.receiveMessageUntil('\n');
            if (rc.status.code == 1) {
                String receivedMessage = rc.data;

                printf("Received: %s\n", receivedMessage.c_str());

                if (receivedMessage == "RELAY_ON") {
                    relay_on();
                }

                if (receivedMessage == "RELAY_OFF") {
                    relay_off();
                }
            }
        }

        vTaskDelay(5);
    }
}

void timeoutTask(void *arg) {
    for (;;) {
        if (relayState == LOW && (millis() - lastCommandTime > TIMEOUT)) {
            relay_off();
        }
        vTaskDelay(100);
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED_INDICATOR, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(PIN_LED_INDICATOR, LOW);

    radio_init();

    xTaskCreatePinnedToCore(buttonTask, "buttonTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(receiveTask, "receiveTask", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(timeoutTask, "timeoutTask", 2048, NULL, 1, NULL, 0);
}

void loop() {
}
