#include <Arduino.h>

void setup() {

  pinMode(8, OUTPUT);
  pinMode(7, INPUT);

  Serial.begin(9600);

  Serial1.begin(9600, SERIAL_8N1, 21, 20);
}

void loop() {

  if (digitalRead(7) == LOW) {

    digitalWrite(8, HIGH);

  } else {

    digitalWrite(8, LOW);

  }

  if (Serial.available()) {      // If anything comes in Serial (USB),

    Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)

  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)

    Serial.write(Serial1.read());   // read it and send it out Serial (USB)

  }
}