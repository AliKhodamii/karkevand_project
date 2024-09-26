#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial sim800(D6, D7);

void setup()
{
    Serial.begin(9600);
    sim800.begin(9600);
}

void loop()
{
    while (Serial.available())
    {
        sim800.println(Serial.readString());
    }
    while (sim800.available())
    {
        Serial.println(sim800.readString());
    }
}