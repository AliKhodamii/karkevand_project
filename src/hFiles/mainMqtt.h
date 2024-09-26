// defines--------------------
#define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
#define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi
#define SerialMon Serial       // For debugging
#define MODEM_RX D6            // RX pin of ESP8266 connected to TX pin of SIM800L
#define MODEM_TX D7            // TX pin of ESP8266 connected to RX pin of SIM800L
#define EEPROM_SIZE 4096
// ---------------------------

// includes-------------------
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <EEPROM.h>
// ---------------------------

// control variables---------
bool valve = false;
bool s_valve = false;
bool restart = false;
bool copy = false;
bool autoIrrigationEn = false;
int humidity = 0;
int duration = 0;
int humidityHighLimit = 0;
int humidityLowLimit = 0;
int lastIrrigationTS = 0;
int nextIrrigationTS = 0;
int howOften = 0;
int hour = 0;
int minute = 0;
// ---------------------------

// time variables-------------
unsigned long int previousTime1s = 0;
unsigned long int previousTime3s = 0;
unsigned long int previousTime5s = 0;

unsigned long int irrigationStartTime = 0;
unsigned long int waitTime = 0;

int rtcYear = 0;
int rtcMonth = 0;
int rtcDay = 0;
int rtcHour = 0;
int rtcMinute = 0;

int lastIrrYear = 0;
int lastIrrMonth = 0;
int lastIrrDay = 0;
int lastIrrHour = 0;
int lastIrrMinute = 0;
//----------------------------

// pin initialize-------------
// 1-valve 2,3-TX RX 4-LowHumidity 5-NormalHumidity 6-sim800reset 7-comPin 8-
const int valvePin = D1;
const int lowHumidityPin = D2;
const int normalHumidityPin = D3;
const int sim800ResetPin = D5;
const int comPin = D8;
const int humidityPin = A0;
// ---------------------------

// function forward declaration------
bool gsmPost();
bool gsmGet();
void dataUpdate();
void dataPrepare();
bool valveOpen();
bool valveClose();
bool autoIrrigation(bool set);
void mqttCallback(char *topic, byte *payload, unsigned int length);
bool mqttConnect();
String myTime();
int rtcTimeDate();
void lastIrrTimeDate();
int createNextIrrTimeStamp();
// int rtcHour();
// int rtcMinute();
// int rtcDay();
void giveInfo();
void getFromEEPROM();
void putToEEPROM();
void blink();
void humidityRead();
//-----------------------------------

// declare json handler
JsonDocument sysInfo;
//--------------------

// Initialize SoftwareSerial for SIM800L----
SoftwareSerial SerialAT(MODEM_RX, MODEM_TX);

// http variables-------------
const char *serverPath = "sed-smarthome.ir";
const String getPath = "/karkevand/information.php";
const String postPath = "/karkevand/poster.php";
String sysInfoJson = "";
//----------------------------

// GPRS credentials-----------
const char apn[] = "rightel"; // Replace with your APN
const char user[] = "";       // GPRS username, if any
const char pass[] = "";       // GPRS password, if any
//----------------------------

// MQTT broker details-----------------------
const char *broker = "test.mosquitto.org";
const int port = 1883; // MQTT port

// Topics for publishing/subscribing
const char *topicInit = "karSSG/ESP";
const char *topicLed = "karSSG/client";
//-------------------------------------------
// init modem and httpClient and mqttClient
TinyGsm modem(SerialAT);
TinyGsmClient httpClient(modem);
TinyGsmClient mqttClient(modem);
HttpClient http(httpClient, serverPath, 80); // Replace with your server address
PubSubClient mqtt(mqttClient);
//------------------------------------------

void setup()
{
    SerialMon.begin(9600); // start serialMon for debug
    delay(10);
    SerialAT.begin(9600); // Start communication with SIM800L at 9600 baud
    delay(3000);

    // define pins mode
    pinMode(valvePin, OUTPUT);
    pinMode(lowHumidityPin, OUTPUT);
    pinMode(normalHumidityPin, OUTPUT);
    pinMode(sim800ResetPin, OUTPUT);
    pinMode(comPin, OUTPUT);

    pinMode(humidityPin, OUTPUT);

    // pins initial values
    digitalWrite(valvePin, 0);
    digitalWrite(lowHumidityPin, 0);
    digitalWrite(normalHumidityPin, 0);
    digitalWrite(sim800ResetPin, 0);
    digitalWrite(comPin, 0);

    // first http get request to get initial values
    SerialMon.println("get data from EEPROM to start...");
    getFromEEPROM();

    // initial values
    valve = false;
    restart = false;
    copy = false;

    // first http post request to post inital values (like valve = false)
    SerialMon.println("put data to EEPROM...");
    putToEEPROM();

    // create last irrigation time date
    lastIrrTimeDate();
    // create next irrigation timestamp
    createNextIrrTimeStamp();

    // if sim800 isn't registered we will wait
    while (!modem.isNetworkConnected())
    {
        // blink twice
        blink();
        blink();

        // start modem (gsm module)
        modem.restart();

        // Print modem information
        String modemInfo = modem.getModemInfo();
        SerialMon.print("Modem Info: ");
        SerialMon.println(modemInfo);
        Serial.println("modem is not connected to network, waiting 5 seconds...");
        delay(5000);
    }
    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        SerialMon.println("modem  network is connected");
        // Connect to the GPRS network
        while (!modem.gprsConnect(apn, user, pass))
        {
            // blink 3 times
            blink();
            blink();
            blink();

            SerialMon.print("Connecting to the GPRS...");
            if (!modem.gprsConnect(apn, user, pass))
            {
                SerialMon.println(" fail");
            }
            SerialMon.println(" success");
        }
    }
    else
    {
        SerialMon.println("modem network is not connected");
    }

    // sync modem time with tehran time
    if (modem.NTPServerSync())
    {
        Serial.println("Time synced successfully");
        Serial.print("Current time: ");
        Serial.println(modem.getGSMDateTime(DATE_FULL)); // Print current time
    }
    else
    {
        Serial.println("Failed to sync time");
    }

    // MQTT broker setup
    mqtt.setServer(broker, port);
    mqtt.setKeepAlive(120);
    mqtt.setCallback(mqttCallback);

    // Attempt to connect to MQTT broker
    SerialMon.print("Connecting to MQTT...");
    if (mqttConnect())
    {
        // blink 4 times
        blink();
        blink();
        blink();
        blink();

        SerialMon.println(" connected");
        mqtt.publish(topicInit, "GsmClientTest started");
        mqtt.subscribe(topicLed);
    }
    else
    {
        SerialMon.println(" connection failed");
    }
}
void loop()
{
    // check if sim800 is connected to network
    if (!modem.isNetworkConnected())
    {
        waitTime = millis();
        while (!modem.isNetworkConnected())
        {
            // blink twice
            blink();
            blink();
            SerialMon.println("modem is not connected to network");
            delay(5000);

            // if Network wasn't connected after 2 mins restart esp
            if ((millis() - waitTime) > 120000)
            {
                ESP.restart();
            }
        }
    }

    // connect to GPRS if we're not
    if (modem.isNetworkConnected() && !modem.isGprsConnected())
    {
        waitTime = millis();
        while (modem.isNetworkConnected() && !modem.isGprsConnected())
        {
            // blink 3 times
            blink();
            blink();
            blink();

            SerialMon.println("modem is not connected to GPRS");
            SerialMon.print("Connecting to the GPRS network...");
            if (!modem.gprsConnect(apn, user, pass))
            {
                SerialMon.println(" fail");
            }
            SerialMon.println(" success");

            // if GPRS wasn't connected after 2 mins restart esp
            if ((millis() - waitTime) > 120000)
            {
                ESP.restart();
            }
        }
    }

    // reconnect to mqtt
    if (!mqtt.connected())
    {
        // blink 4 times
        blink();
        blink();
        blink();
        blink();

        SerialMon.print("Reconnecting to MQTT...");
        if (mqttConnect())
        {
            SerialMon.println(" connected");
            mqtt.publish(topicInit, "GsmClientTest started");
            mqtt.subscribe(topicLed);
        }
        else
        {
            SerialMon.println(" reconnection failed");
            delay(10000); // Retry every 10 seconds
        }
    }

    // mqtt loop
    mqtt.loop();

    // (every second) check if valve is 1 & duration is finished or not
    if (millis() - previousTime1s > 1000 && valve)
    {
        Serial.println("check if valve is open and duration is finished");
        if (valve && ((millis() - irrigationStartTime) / 1000) > duration * 60)
        {
            Serial.println("irrigation time is finished, closing valve...");
            valveClose();
            giveInfo();
        }
        previousTime1s = millis();
    }

    // (every minutes) check if autoIrrigation is on or off & check if it's time or not
    if (millis() - previousTime5s > 5000 && autoIrrigationEn && !valve)
    {
        // update rtc time and date
        int currentTimestamp = rtcTimeDate();
        // check if its irrigation time
        Serial.println("Checking if it's irrigation time");
        if ((currentTimestamp > nextIrrigationTS))
        {
            Serial.println("auto Irrigation will happen now...");
            if (valveOpen())
            {
                giveInfo();
            }
        }
        else if ((currentTimestamp - lastIrrigationTS) > (howOften * 24 * 60 * 60))
        {
            Serial.println("irrigation day , not irrigation hour yet");
        }
        else
        {
            Serial.println("it's not auto irrigation day");
        }
        previousTime5s = millis();
    }

    // blink every 3 sec and show system is ok
    if (millis() - previousTime3s > 3000)
    {
        blink();
        previousTime3s = millis();
    }
}

bool gsmPost()
{
    Serial.println("post request...");
    // check modem signal quality
    int signalQuality = modem.getSignalQuality();
    SerialMon.print("Signal quality: ");
    SerialMon.println(signalQuality);

    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        SerialMon.println("modem network is connected");

        // check if modem gprs is connected
        if (modem.isGprsConnected())
        {
            SerialMon.println("modem GPRS is connected");

            // connect to server
            if (!httpClient.connected())
            {
                SerialMon.println("Connecting to server...");
                if (!httpClient.connect(serverPath, 80))
                { // Replace with your server address
                    SerialMon.println(" fail");
                    return false;
                }
                SerialMon.println(" success");
            }

            // prepare data to be posted
            dataPrepare();

            // Set the request headers
            http.beginRequest();
            http.post(postPath);
            http.sendHeader("Content-Type", "application/x-www-form-urlencoded");
            http.sendHeader("Content-Length", sysInfoJson.length() + 5);
            // http.sendHeader("X-Custom-Header", "custom-header-value");
            http.beginBody();
            SerialMon.println("");
            SerialMon.println("sent data:");
            SerialMon.println(sysInfoJson);
            SerialMon.println("");

            http.print("info=" + sysInfoJson);
            http.endRequest();
            SerialMon.println("POST request sent");

            // Read the response
            int statusCode = http.responseStatusCode();
            String response = http.responseBody();

            SerialMon.print("Status code: ");
            SerialMon.println(statusCode);
            SerialMon.print("Response: ");
            SerialMon.println(response);
            // Close the connection
            http.stop();
            SerialMon.println("Server disconnected");

            if (statusCode == 200)
                return true;
            else
                return false;
        }
        else
        {
            SerialMon.println("modem GPRS is not connected");

            // try 2 time to connect modem GPRS
            for (int i = 1; i <= 2; i++)
            {
                SerialMon.print("trying to connect modem GPRS, try number: ");
                SerialMon.println(i);
                // Connect to the GPRS network
                SerialMon.print("Connecting to the network...");
                if (modem.gprsConnect(apn, user, pass))
                {
                    SerialMon.println(" success");
                    break;
                }
                SerialMon.println(" fail");
                SerialMon.println("unable to connect modem GPRS");
            }
        }
    }
    else
    {
        SerialMon.println("modem network is not connected");
        return false;
    }
    return false;
}
bool gsmGet()
{
    SerialMon.println("get request...");
    if (!httpClient.connected())
    {
        SerialMon.println("Connecting to server...");
        if (!httpClient.connect(serverPath, 80))
        { // Replace with your server address
            SerialMon.println(" fail");
            delay(10000);
            return "ERROR";
        }
        SerialMon.println(" success");
    }

    int statusCode = 0;
    int tryNum = 0;
    while (statusCode != 200)
    {
        tryNum++;
        // Send the HTTP GET request
        http.get(getPath); // Replace with your resource path

        // Read the response status code
        statusCode = http.responseStatusCode();
        SerialMon.print("Response status code: ");
        SerialMon.println(statusCode);

        if (tryNum > 3)
        {
            return false;
        }
        if (statusCode == 200)
        {
            break;
        }
        delay(1000);
    }

    // Read the response body
    String response = http.responseBody();
    SerialMon.println("Response:");
    SerialMon.println(response);
    // Close the connection
    http.stop();
    SerialMon.println("Server disconnected");

    // deserialize
    deserializeJson(sysInfo, response);

    // update latest data
    dataUpdate();

    return true;
}
void dataUpdate()
{
    s_valve = sysInfo["valve"];
    restart = sysInfo["restart"];
    copy = sysInfo["copy"];
    humidity = sysInfo["humidity"];
    duration = sysInfo["duration"];
    humidityHighLimit = sysInfo["humidityHighLimit"];
    humidityLowLimit = sysInfo["humidityLowLimit"];
    lastIrrigationTS = sysInfo["lastIrrigationTS"];
    autoIrrigationEn = sysInfo["autoIrrigationEn"];
    howOften = sysInfo["howOften"];
    hour = sysInfo["hour"];
    minute = sysInfo["minute"];
}
void dataPrepare()
{
    humidityRead();

    sysInfo["workingTime"] = myTime();
    sysInfo["valve"] = valve;
    sysInfo["restart"] = restart;
    sysInfo["humidity"] = humidity;
    sysInfo["copy"] = copy;
    sysInfo["duration"] = duration;
    sysInfo["humidityHighLimit"] = humidityHighLimit;
    sysInfo["humidityLowLimit"] = humidityLowLimit;
    sysInfo["lastIrrigationTS"] = lastIrrigationTS;
    sysInfo["autoIrrigationEn"] = autoIrrigationEn;
    sysInfo["humidity"] = humidity;
    sysInfo["howOften"] = howOften;
    sysInfo["hour"] = hour;
    sysInfo["minute"] = minute;

    sysInfoJson = "";
    serializeJson(sysInfo, sysInfoJson);
}
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // blink when message arrived
    blink();

    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, length);
    SerialMon.println();

    String message = "";
    for (uint i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    // do commands
    if (message.indexOf("valve open") != -1)
    {
        SerialMon.println("valve open command is received\nopening valve");
        if (message.substring(message.indexOf("open") + 5) != "")
        {
            duration = message.substring(message.indexOf("open") + 5).toInt();
            Serial.println("duration updated");
        }
        if (valveOpen())
            mqtt.publish(topicInit, "valve is open");
        else
            mqtt.publish(topicInit, "valve is in prev mode");
    }
    else if (message == "valve close")
    {
        SerialMon.println("valve close command is received\nclosing valve");
        if (valveClose())
        {
            mqtt.publish(topicInit, "valve is close");
        }
        else
        {
            mqtt.publish(topicInit, "valve is in prev mode");
        }
    }
    else if (message == "give info")
    {
        dataPrepare();
        // sysInfoJson = "info=" + sysInfoJson;
        Serial.println(sysInfoJson.c_str());
        mqtt.publish(topicInit, sysInfoJson.c_str());
    }
    else if (message == "autoIrrigationOn")
    {
        autoIrrigationEn = 1;
        putToEEPROM();
        createNextIrrTimeStamp();
        mqtt.publish(topicInit, "autoIrrigation is on");
    }
    else if (message == "autoIrrigationOff")
    {
        autoIrrigationEn = 0;
        putToEEPROM();
        mqtt.publish(topicInit, "autoIrrigation is off");
    }
    else if (message.indexOf("set Irr time to") != -1)
    {
        String SHour = message.substring(message.indexOf("to") + 3, message.indexOf("to") + 5);
        hour = SHour.toInt();
        Serial.println(SHour);
        String SMinute = message.substring(message.indexOf("to") + 6);
        minute = SMinute.toInt();
        Serial.println(SMinute);
        putToEEPROM();
        createNextIrrTimeStamp();
        mqtt.publish(topicInit, "hour is set to what you wanted");
    }
    else if (message.indexOf("set howOften to") != -1)
    {
        String SDay = message.substring(message.indexOf("to") + 3);
        howOften = SDay.toInt();
        putToEEPROM();
        createNextIrrTimeStamp();
        mqtt.publish(topicInit, "how Often is setted");
    }
}
bool mqttConnect()
{
    return mqtt.connect("karSSG"); // You can add MQTT username/password if required
}
bool valveOpen()
{
    Serial.println("Valve Opening...");
    valve = true;
    digitalWrite(valvePin, valve);
    irrigationStartTime = millis();
    lastIrrigationTS = rtcTimeDate();
    putToEEPROM();
    createNextIrrTimeStamp();
    return 1;
}
bool valveClose()
{
    Serial.println("Valve Closing...");
    valve = false;
    digitalWrite(valvePin, valve);
    return 1;
};
String myTime()
{
    int days = millis() / 86400000;
    int hours = (millis() - days * 86400000) / 3600000;
    int minutes = ((millis() - days * 86400000) - hours * 3600000) / 60000;
    int seconds = (millis() / 1000) % 60;
    return String(days) + "d :" + String(hours) + "h :" + String(minutes) + "m :" + String(seconds) + "s";
}
int rtcTimeDate()
{
    String timeString = modem.getGSMDateTime(DATE_FULL);
    Serial.println(timeString);                          // Print current time
    rtcYear = timeString.substring(0, 2).toInt() + 2000; // Convert to full year (2024)
    rtcMonth = timeString.substring(3, 5).toInt();
    rtcDay = timeString.substring(6, 8).toInt();
    rtcHour = timeString.substring(9, 11).toInt();
    rtcMinute = timeString.substring(12, 14).toInt();
    int second = timeString.substring(15, 17).toInt();
    int timezoneOffset = timeString.substring(18, 21).toInt(); // Timezone in hours

    // // Print extracted values
    // Serial.print("Year: ");
    // Serial.println(year);
    // Serial.print("Month: ");
    // Serial.println(month);
    // Serial.print("Day: ");
    // Serial.println(day);
    // Serial.print("Hour: ");
    // Serial.println(hour);
    // Serial.print("Minute: ");
    // Serial.println(minute);
    // Serial.print("Second: ");
    // Serial.println(second);
    // Serial.print("Timezone offset: ");
    // Serial.println(timezoneOffset);

    // Populate the tm structure
    struct tm tm;
    tm.tm_year = rtcYear - 1900; // Year since 1900
    tm.tm_mon = rtcMonth - 1;    // Month (0-11)
    tm.tm_mday = rtcDay;         // Day of the month
    tm.tm_hour = rtcHour;        // Hour (0-23)
    tm.tm_min = rtcMinute;       // Minutes (0-59)
    tm.tm_sec = second;          // Seconds (0-59)
    tm.tm_isdst = 0;             // Daylight saving time flag (not used)

    // Convert to timestamp (seconds since Jan 1, 1970)
    time_t t = mktime(&tm);

    // Print the Unix timestamp
    Serial.print("Unix Timestamp: ");
    Serial.println(t);
    return t;
}
void lastIrrTimeDate()
{
    time_t timestamp = lastIrrigationTS;
    // Convert timestamp to a struct tm for breakdown
    struct tm *timeInfo;

    // Convert timestamp to UTC time
    timeInfo = gmtime(&timestamp); // Use localtime() for local time if timezone is set

    // Extract year, month, day, hour, and minute
    lastIrrYear = timeInfo->tm_year + 1900; // Years since 1900
    lastIrrMonth = timeInfo->tm_mon + 1;    // Months are 0-11
    lastIrrDay = timeInfo->tm_mday;         // Day of the month
    lastIrrHour = timeInfo->tm_hour;        // Hours since midnight (0-23)
    lastIrrMinute = timeInfo->tm_min;       // Minutes after the hour

    // Print the extracted values
    Serial.println("last Irr Time and date");
    Serial.print("Year: ");
    Serial.println(lastIrrYear);
    Serial.print("Month: ");
    Serial.println(lastIrrMonth);
    Serial.print("Day: ");
    Serial.println(lastIrrDay);
    Serial.print("Hour: ");
    Serial.println(lastIrrHour);
    Serial.print("Minute: ");
    Serial.println(lastIrrMinute);
}
int createNextIrrTimeStamp()
{
    nextIrrigationTS = lastIrrigationTS + (howOften * 24 * 60 * 60);
    time_t timestamp = nextIrrigationTS;
    // Convert timestamp to a struct tm for breakdown
    struct tm *timeInfo;

    // Convert timestamp to UTC time
    timeInfo = gmtime(&timestamp); // Use localtime() for local time if timezone is set

    // Extract year, month, day, hour, and minute
    int year = timeInfo->tm_year + 1900; // Years since 1900
    int month = timeInfo->tm_mon + 1;    // Months are 0-11
    int day = timeInfo->tm_mday;         // Day of the month
    // int hour = hour;
    // int minute = minute;
    int second = 0;

    Serial.print("next irrTS: ");
    Serial.print(year);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minute);

    struct tm tm;
    tm.tm_year = year - 1900; // Year since 1900
    tm.tm_mon = month - 1;    // Month (0-11)
    tm.tm_mday = day;         // Day of the month
    tm.tm_hour = hour;        // Hour (0-23)
    tm.tm_min = minute;       // Minutes (0-59)
    tm.tm_sec = second;       // Seconds (0-59)
    tm.tm_isdst = 0;

    // Convert to timestamp (seconds since Jan 1, 1970)
    time_t t = mktime(&tm);

    // Print the Unix timestamp
    Serial.print("next irrigation timestamp: ");
    Serial.println(t);
    nextIrrigationTS = t;
    return t;
}
// // int rtcHour()
// {
//     String timeString = modem.getGSMDateTime(DATE_FULL);
//     int hour = timeString.substring(9, 11).toInt();
//     return hour;
// }
// int rtcMinute()
// {
//     String timeString = modem.getGSMDateTime(DATE_FULL);
//     int minute = timeString.substring(12, 14).toInt();
//     return minute;
// }
// int rtcDay()
// {
//     String timeString = modem.getGSMDateTime(DATE_FULL);
//     int day = timeString.substring(6, 8).toInt();
//     return day;
// }

void giveInfo()
{
    dataPrepare();
    sysInfoJson = "info=" + sysInfoJson;
    mqtt.publish(topicInit, sysInfoJson.c_str());
}
void getFromEEPROM()
{
    EEPROM.begin(EEPROM_SIZE);

    Serial.println("loading data...");
    char ch;
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        ch = EEPROM.read(i);
        if (ch == '\0')
            break; // Stop reading if null terminator is found
        sysInfoJson += ch;
    }

    Serial.println("Data loaded from EEPROM:");
    Serial.println(sysInfoJson);
    deserializeJson(sysInfo, sysInfoJson);
    dataUpdate();
    EEPROM.end();
}
void putToEEPROM()
{
    dataPrepare();
    EEPROM.begin(EEPROM_SIZE);
    Serial.println("putting data to EEPROM");
    int len = sysInfoJson.length();
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(i, sysInfoJson[i]);
    }
    EEPROM.write(len, '\0');
    if (EEPROM.commit())
        Serial.println("Data saved to EEPROM.");
    EEPROM.end();
}
void blink()
{
    digitalWrite(comPin, 1);
    delay(250);
    digitalWrite(comPin, 0);
    delay(250);
}
void humidityRead()
{
    // humidity = analogRead(humidityPin);
    for (int i = 0; i < 10; i++)
    {
        humidity = humidity + analogRead(humidityPin);
        delay(10);
    }
    humidity = humidity / 10;
}
