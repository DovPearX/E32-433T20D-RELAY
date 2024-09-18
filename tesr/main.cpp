/*

  This example shows how to connect to an EBYTE transceiver
  using an ESP32


  This example shows how to connect to an EBYTE transceiver
  using an ESP32

  This code for for the sender

  ESP32 won't allow SoftwareSerial (at least I can't get that lib to work
  so you will just hardwire your EBYTE directly to the Serial2 port


*/

#include "EBYTE.h"


/*
WARNING: IF USING AN ESP32
DO NOT USE THE PIN NUMBERS PRINTED ON THE BOARD
YOU MUST USE THE ACTUAL GPIO NUMBER
*/
#define PIN_RX 20   // Serial2 RX (connect this to the EBYTE Tx pin)
#define PIN_TX 21   // Serial2 TX pin (connect this to the EBYTE Rx pin)

#define PIN_M0 5   // D4 on the board (possibly pin 24)
#define PIN_M1 6   // D2 on the board (possibly called pin 22)
#define PIN_AX 7   // D15 on the board (possibly called pin 21)


// i recommend putting this code in a .h file and including it
// from both the receiver and sender modules
struct DATA {
  unsigned long Count;
  int Bits;
  float Volts;
  float Amps;

};

// these are just dummy variables, replace with your own
int Chan;
DATA MyData;
unsigned long Last;


// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&Serial1, PIN_M0, PIN_M1, PIN_AX);

void setup() {

  // set up the serial por
  delay(3000);

  Serial.begin(9600);

  Serial1.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);

  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Serial.println(Transceiver.init());

  // all these calls are optional but shown to give examples of what you can do

  Serial.println(Transceiver.GetAirDataRate());
  Serial.println(Transceiver.GetChannel());
  Transceiver.SetAddressH(1);
  Transceiver.SetAddressL(1);
  Chan = 15;
  Transceiver.SetChannel(Chan);
  Transceiver.SaveParameters(PERMANENT);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();

}

void loop() {

}