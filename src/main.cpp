#include "Arduino.h"
#include "radio.h"

#define PIN_BUTTON 9
#define PIN_LED_INDICATOR 10

#define DEBOUNCE_TIME 50
#define LONG_PRESS_TIME 1000

bool autosendActive = false;

void autosend(void *arg) {
    for (;;) {
        if(autosendActive) {
            e32ttl100.sendMessage("Hello World!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void blink_led_indicator() {
    digitalWrite(PIN_LED_INDICATOR, HIGH);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    digitalWrite(PIN_LED_INDICATOR, LOW);
}

void blink_led_indicator_double() {
    digitalWrite(PIN_LED_INDICATOR, HIGH);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    digitalWrite(PIN_LED_INDICATOR, LOW);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    digitalWrite(PIN_LED_INDICATOR, HIGH);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    digitalWrite(PIN_LED_INDICATOR, LOW);
}

void doclickaction() {
    printf("SingleClick\n");
    e32ttl100.sendMessage("SINGLE\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    blink_led_indicator();
}

void dolongclickaction() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    autosendActive = !autosendActive;
    printf("Autosend active: %s\n", autosendActive ? "true" : "false");
    if(autosendActive) {
        blink_led_indicator_double();
    } else {
        blink_led_indicator();
    }
}

void buttonTask(void *arg) {
    static unsigned long pressStartTime = 0;
    static bool buttonHeld = false;

    for (;;) {
        bool buttonState = digitalRead(PIN_BUTTON) == LOW;

        if (buttonState) {
            if (!buttonHeld) {
                pressStartTime = millis();
                buttonHeld = true;
            } else {
                if (millis() - pressStartTime >= LONG_PRESS_TIME) {
                    dolongclickaction();
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                    buttonHeld = false;
                }
            }
        } else {
            if (buttonHeld) {
                doclickaction();
                buttonHeld = false;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED_INDICATOR, OUTPUT);
    digitalWrite(PIN_LED_INDICATOR, LOW);

    radio_init();

    xTaskCreatePinnedToCore(buttonTask, "buttonTask", 2048, NULL, 1, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(autosend, "autosend", 2048, NULL, 2, NULL, tskNO_AFFINITY);
}

void loop() {
}
