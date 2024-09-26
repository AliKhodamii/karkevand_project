#include <EEPROM.h>
#include <ArduinoJson.h>

#define EEPROM_SIZE 4096
JsonDocument doc;
int address = 0;
byte value;

void setup()
{
    Serial.begin(9600);
    delay(5000);
    Serial.println("Creating data...");
    EEPROM.begin(EEPROM_SIZE);

    doc["workingTime"] = "";
    doc["valve"] = true;
    doc["restart"] = true;
    doc["copy"] = false;
    doc["humidity"] = 0;
    doc["duration"] = 2;
    doc["humidityHighLimit"] = 0;
    doc["humidityLowLimit"] = 0;
    doc["lastIrrigationTS"] = 1725646670;
    doc["autoIrrigationEn"] = false;
    doc["howOften"] = 3;
    doc["hour"] = 21;
    doc["minute"] = 0;

    Serial.println("convert to json");
    String json = "";
    serializeJson(doc, json);

    Serial.println("putting data to eeprom");
    int len = json.length();
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(i, json[i]);
    }
    EEPROM.write(len, '\0');
    EEPROM.commit();
    Serial.println("Data saved to EEPROM.");

    Serial.println("");
    Serial.println("");
    Serial.println("");

    Serial.println("loading data...");
    String jsonString;
    char ch;
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        ch = EEPROM.read(i);
        if (ch == '\0')
            break; // Stop reading if null terminator is found
        jsonString += ch;
    }

    Serial.println("Data loaded from EEPROM:");
    Serial.println(jsonString);
}

void loop()
{
}