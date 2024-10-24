#pragma once

#include "LoRa_E32.h"

#define PIN_RX 20
#define PIN_TX 21
#define PIN_AUX 7
#define PIN_M0 5
#define PIN_M1 6

LoRa_E32 e32ttl100(PIN_TX, PIN_RX, &Serial1, PIN_AUX, PIN_M0, PIN_M1, UART_BPS_RATE_9600, SERIAL_8N1);

void radio_init() {
    pinMode(PIN_AUX, INPUT);

    e32ttl100.begin();

    ResponseStructContainer c;
    c = e32ttl100.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;

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

    ResponseStatus rs = e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);
    Serial.println(rs.getResponseDescription());
    Serial.println(rs.code);
    c.close();

}