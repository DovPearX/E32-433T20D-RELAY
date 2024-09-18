#include <Arduino.h>

int timestamp = 0;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(7, INPUT);

  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 21, 20); // Assuming you're using pins 21 (RX) and 20 (TX)

  delay(1000);

  // Send initial bytes to Serial1
  Serial1.write(0xC0);
  Serial1.write(0x01);
  Serial1.write(0x01);
  Serial1.write(0x1A);
  Serial1.write(0x17);
  Serial1.write(0x47);

  delay(1000);
}

void loop() {
  Serial1.flush();
  delay(1000);

  Serial1.write("Hello");
  Serial1.write(timestamp);
  Serial1.write("\n");
  timestamp ++;

  // Check if there's data available on Serial1
  if (Serial1.available()) {
    char buffer[64]; // Buffer to hold incoming data
    int len = Serial1.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
    buffer[len] = '\0'; // Null-terminate the string
    Serial.println(buffer); // Print the received data on Serial
  }
}
