#include "Arduino.h"
#include "LoRa_E32.h"

#define PIN_BUTTON 9     // Przycisk licznika na pinie 9
#define PIN_LED_INDICATOR 10  // LED wskaźnik na pinie 10
#define PIN_RX 20
#define PIN_TX 21
#define PIN_AUX 7
#define PIN_M0 6
#define PIN_M1 5
#define PIN_DIODE 8  // Pin dla diody
#define DEBOUNCE_TIME 1000  // Krótszy czas debounce

volatile bool buttonPressed = false;
volatile bool dataReceived = false;  // Flaga do oznaczania odebrania danych
volatile unsigned long counter = 0;  // Licznik dla przycisku

unsigned long lastDebounceTime = 0;  // Ostatni czas naciśnięcia przycisku

LoRa_E32 e32ttl100(PIN_TX, PIN_RX, &Serial1, PIN_AUX, PIN_M0, PIN_M1, UART_BPS_RATE_9600, SERIAL_8N1);

void IRAM_ATTR handleButton() {
  unsigned long currentTime = millis();
  // Sprawdź debounce
  if (currentTime - lastDebounceTime > DEBOUNCE_TIME) {
    buttonPressed = true;
    lastDebounceTime = currentTime;
  }
}

void blink_led_indicator(bool longBlink = false) {
  int delayTime = longBlink ? 1000 : 100;  // Dłuższy czas migania jeśli longBlink jest true
  digitalWrite(PIN_LED_INDICATOR, HIGH);
  delay(delayTime);
  digitalWrite(PIN_LED_INDICATOR, LOW);
  delay(delayTime);
}

void blink_diode(int times, int delayTime = 100) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_DIODE, HIGH);
    delay(delayTime);
    digitalWrite(PIN_DIODE, LOW);
    delay(delayTime);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED_INDICATOR, OUTPUT);  // LED wskaźnik jako OUTPUT
  pinMode(PIN_DIODE, OUTPUT);  // Pin diody jako OUTPUT
  pinMode(PIN_AUX, INPUT);  // Pin AUX jako INPUT

  digitalWrite(PIN_LED_INDICATOR, LOW);  // LED wskaźnik na początku wyłączony
  digitalWrite(PIN_DIODE, LOW);  // Diode na początku wyłączona

  delay(500);

  e32ttl100.begin();

  ResponseStructContainer c;
	c = e32ttl100.getConfiguration();
	// It's important get configuration pointer before all other operation
	Configuration configuration = *(Configuration*) c.data;
	Serial.println(c.status.getResponseDescription());
	Serial.println(c.status.code);

	configuration.ADDL = 0x0;
	configuration.ADDH = 0x1;
	configuration.CHAN = 0x19;

	configuration.OPTION.fec = FEC_0_OFF;
	configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
	configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
	configuration.OPTION.transmissionPower = POWER_20;
	configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;

	configuration.SPED.airDataRate = AIR_DATA_RATE_000_03;
	configuration.SPED.uartBaudRate = UART_BPS_9600;
	configuration.SPED.uartParity = MODE_00_8N1;

	// Set configuration changed and set to not hold the configuration
	ResponseStatus rs = e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);
	Serial.println(rs.getResponseDescription());
	Serial.println(rs.code);
	c.close();

  // Ustawienie przerwania dla przycisku
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), handleButton, FALLING);

  // Wyślij wiadomość testową
  rs = e32ttl100.sendMessage("Hello, world?");
  Serial.println(rs.getResponseDescription());
}

void loop() {
  // Sprawdzanie stanu pinu AUX i sterowanie diodą
  if (digitalRead(PIN_AUX) == HIGH) {
    digitalWrite(PIN_DIODE, HIGH);  // Zapal diodę
  } else {
    digitalWrite(PIN_DIODE, LOW);  // Zgaś diodę
  }

  // Obsługa zdarzenia przerwania w głównej pętli
  if (buttonPressed) {
    buttonPressed = false;  // Zresetuj flagę
    counter++;

    // Wysyłanie wartości licznika przez LoRa
    String message = "Counter: " + String(counter);
    ResponseStatus rs = e32ttl100.sendMessage(message);
    Serial.println(message);

    // Migaj LED wskaźnik tylko przy poprawnym wysłaniu wiadomości
    if (rs.code == 1) {  // Sprawdź, czy wysłanie było udane
      blink_led_indicator();
    } else {
      Serial.print("Send failed: ");
      Serial.println(rs.getResponseDescription());
      blink_diode(5, 200);  // Migaj diodą 5 razy przy niepowodzeniu
    }
  }

  // Sprawdzanie wiadomości przychodzących
  if (e32ttl100.available()) {
    // Odczyt wiadomości
    ResponseContainer rc = e32ttl100.receiveMessage();
    if (rc.status.code != 1) {
      Serial.print("Receive failed: ");
      Serial.println(rc.status.getResponseDescription());
    } else {
      Serial.println(rc.data);
      dataReceived = true;  // Ustaw flagę odbioru danych
    }
  }

  // Miganie LED dłużej, jeśli dane zostały odebrane
  if (dataReceived) {
    dataReceived = false;  // Zresetuj flagę
    blink_led_indicator(true);  // Dłuższe miganie
  }

  // Wysyłanie wiadomości z terminala
  if (Serial.available()) {
    String input = Serial.readString();
    e32ttl100.sendMessage(input);
  }
}
