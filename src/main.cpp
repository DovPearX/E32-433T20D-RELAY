#include "Arduino.h"
#include "radio.h"

#define PIN_BUTTON 10
#define PIN_LED_INDICATOR 8
#define RELAY_PIN 3
#define TIMEOUT 3000  // 3 sekundy
#define DEBOUNCE_DELAY 50 // Opóźnienie debouncingu w milisekundach
#define SEND_DELAY 1000  // Opóźnienie między wysyłaniem wiadomości w milisekundach

#define SENDER  // Użyj tego, aby skompilować jako nadajnik

bool relayState = HIGH;
unsigned long lastCommandTime = 0; // Czas ostatniej komendy
unsigned long lastButtonPressTime = 0; // Czas ostatniego wciśnięcia przycisku
unsigned long lastSendTime = 0; // Czas ostatniego wysłania wiadomości

void relay_on() {
    if (relayState == HIGH) {
        digitalWrite(RELAY_PIN, LOW); // Włącz przekaźnik (LOW)
        relayState = LOW;
        printf("(%lu ms) Relay ON\n", lastCommandTime);
    }

    lastCommandTime = millis(); // Aktualizuj czas ostatniej komendy
}

void relay_off() {
    if (relayState == LOW) {
        digitalWrite(RELAY_PIN, HIGH); // Wyłącz przekaźnik (HIGH)
        relayState = HIGH;
        printf("(%lu ms) Relay OFF\n", millis());
    }
}

void buttonTask(void *arg) {
    for (;;) {
        bool buttonState = digitalRead(PIN_BUTTON);
        unsigned long currentTime = millis();

        if (buttonState == LOW && (currentTime - lastButtonPressTime > DEBOUNCE_DELAY)) {
            relay_on(); // Włącz przekaźnik

            if (currentTime - lastSendTime > SEND_DELAY) {
                e32ttl100.sendMessage("RELAY_ON\n"); // Wysłanie komunikatu
                lastSendTime = currentTime; // Zaktualizuj czas ostatniego wysłania
            }
            lastButtonPressTime = currentTime; // Zaktualizuj czas ostatniego wciśnięcia
        } else if (buttonState == HIGH) {
            // Gdy przycisk jest zwolniony, zresetuj ostatni czas przycisku
            lastButtonPressTime = currentTime;
        }

        vTaskDelay(10); // Zmniejsz opóźnienie w celu poprawy responsywności
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
                    relay_on(); // Włącz przekaźnik i zresetuj licznik
                }

                if (receivedMessage == "RELAY_OFF") {
                    relay_off(); // Wyłącz przekaźnik
                }
            }
        }

        vTaskDelay(5); // Krótkie opóźnienie, aby nie przeciążać CPU
    }
}

void timeoutTask(void *arg) {
    for (;;) {
        // Wyłącz przekaźnik, jeśli nie otrzymano komendy przez TIMEOUT
        if (relayState == LOW && (millis() - lastCommandTime > TIMEOUT)) {
            relay_off(); // Wyłącz przekaźnik po upływie czasu
        }
        vTaskDelay(100); // Opóźnienie w celu zmniejszenia obciążenia CPU
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED_INDICATOR, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(RELAY_PIN, HIGH); // Domyślnie przekaźnik jest wyłączony (HIGH)
    digitalWrite(PIN_LED_INDICATOR, LOW);

    radio_init(); // Inicjalizacja radia

    // Utworzenie tasków
    xTaskCreatePinnedToCore(buttonTask, "buttonTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(receiveTask, "receiveTask", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(timeoutTask, "timeoutTask", 2048, NULL, 1, NULL, 0);
}

void loop() {
    // Pusty loop, logika działa w taskach FreeRTOS
}
