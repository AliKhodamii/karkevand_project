#include <Arduino.h>
#define MODEM_RX D6 // RX pin of ESP8266 connected to TX pin of SIM800L
#define MODEM_TX D5 // TX pin of ESP8266 connected to RX pin of SIM800L
const int valvePin = D3;
const int lowHumidityPin = D1;
const int normalHumidityPin = D2;
const int sim800ResetPin = D7;
const int comPin = D0;
const int humidityPin = A0;
