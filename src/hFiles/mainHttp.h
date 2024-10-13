// defines--------------------
#define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
#define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi
#define SerialMon Serial       // For debugging
#define MODEM_RX D7            // RX pin of ESP8266 connected to TX pin of SIM800L
#define MODEM_TX D6            // TX pin of ESP8266 connected to RX pin of SIM800L
#define EEPROM_SIZE 4096
// ---------------------------

// includes-------------------
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
// ---------------------------

// path variables------------------
const char *serverPath = "sed-smarthome.ir";
const String sysPath = "/karkevand/httpProt/getInfo.php";
const String cmdPath = "/karkevand/httpProt/postInfo.php";
String sysInfoJson = "";
String cmdInfojson = "";

// GPRS credentials
const char apn[] = "rightel"; // Replace with your APN
const char user[] = "";       // GPRS username, if any
const char pass[] = "";       // GPRS password, if any

// control variables
bool valve = false;
bool restart = false;
bool copy = false;
bool autoIrrEn = false;

int humidity = 0;
int duration = 0;
int humHiLi = 0;
int humLoLi = 0;
int lastIrrTS = 0;
int nextIrrTS = 0;
int howOften = 0;
int hour = 0;
int minute = 0;

// command variables
bool valveCmd = false;
bool restartCmd = false;
bool autoIrrEnCmd = false;

int durationCmd = 0;
int humHiLiCmd = 0;
int humLoLiCmd = 0;
int howOftenCmd = 0;
int hourCmd = 0;
int minuteCmd = 0;

// ---------------------------

// pin initialize-------------
// 1-valve 2,3-TX RX 4-LowHumidity 5-NormalHumidity 6-sim800reset 7-comPin 8-
const int valvePin = D1;
const int lowHumidityPin = D2;
const int normalHumidityPin = D3;
const int sim800ResetPin = D5;
const int comPin = D0;
const int humidityPin = A0;
// ---------------------------

// time variables-----------------
unsigned long waitTime = 0;
unsigned long copyTimer = 0;
unsigned long irrStartTime = 0;
unsigned long loop1sec = 0;
unsigned long loop3sec = 0;
unsigned long loop5sec = 0;

int rtcYear = 0;
int rtcMonth = 0;
int rtcDay = 0;
int rtcHour = 0;
int rtcMinute = 0;

// Initialize SoftwareSerial for SIM800L
SoftwareSerial SerialAT(MODEM_RX, MODEM_TX);

// init modem and http client
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, serverPath, 80); // Replace with your server address

// function forward declaration------
void netConnect();
void gprsConnect();
bool gsmPost(String postData, String path);
bool gsmGet();
void dataUpdate();
String dataPrepareForSys();
String dataPrepareForCmd(bool valveSt);
void sysRestart();
void blink();
void getFromEEPROM();
bool valveOpen();
int rtcTimeDate();
void putToEEPROM();
int createNextIrrTimeStamp();
bool valveClose();
//-----------------------------------

// declare json handler
JsonDocument sysInfo, cmdInfo;

void setup()
{
    SerialMon.begin(9600); // start serialMon for debug
    delay(10);
    SerialAT.begin(9600); // Start communication with SIM800L at 9600 baud
    delay(3000);

    // get initial data from EEPROM
    getFromEEPROM();

    // pins initial values
    pinInit();

    // if sim800 isn't registered we will wait
    netConnect();

    // check if modem is connected to network
    gprsConnect();

    // initial values
    valve = false;
    restart = false;
    copy = false;

    // post starting message to sysInfo & cmdInfo to go to 0 position
    gsmPost(dataPrepareForCmd(false), cmdPath);
    gsmPost(dataPrepareForSys(), sysPath);
}
void loop()
{
    // check if sim800 is connected to network
    netConnect();

    // connect to GPRS if we're not
    gprsConnect();

    // every 5 sec get data from CMD file and post data to SYS file
    if (millis() - loop5sec > 5000)
    {
        bool getSuccess = false;
        // get data from cmd file
        getSuccess = gsmGet();

        // check for valve open cmd
        if (!valve && valveCmd)
        {
            Serial.println("Valve Close cmd, closing valve...");
            valveOpen();
        }

        // check for valve close cmd
        if (valve && !valveCmd)
        {
            Serial.println("Valve Open cmd, closing valve...");
            // valveClose();
        }

        // send gsmpost request if get was successful
        if (getSuccess)
        {
            gsmPost(dataPrepareForSys(), sysPath);
        }

        loop5sec = millis();
    }

    // check if duration is finished every 1 sec
    if (millis() - loop1sec > 1000)
    {
        loop1sec = millis();
        if (valve && (millis() - irrStartTime) > duration * 1000)
        {
            Serial.println("Duration finished, Closing valve...");
            // valveClose();
        }
    }

    // check if it's autoIrr time every 3 sec
    if (millis() - loop3sec > 3000 && autoIrrEn)
    {
        loop3sec = millis();
        int currentTimestamp = rtcTimeDate();
        if ((currentTimestamp > nextIrrTS))
        {
            Serial.println("auto Irrigation will happen now...");
            valveOpen();
        }
        else if ((currentTimestamp - lastIrrTS) > (howOften * 24 * 60 * 60))
        {
            Serial.println("irrigation day , not irrigation time yet");
        }
        else
        {
            Serial.println("it's not auto irrigation day");
        }
    }

    // check copy time
    if (millis() - copyTimer > 20)
    {
        copy = 0;
        gsmPost(dataPrepareForSys(), sysPath);
    }
}

void netConnect()
{

    // if sim800 isn't registered we will wait
    if (!modem.isNetworkConnected())
    {
        waitTime = millis();
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

            // if network didn't connect in 2 mins
            if (millis() - waitTime > 120000)
            {
                sysRestart();
            }
        }
    }
}
void gprsConnect()
{

    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        waitTime = millis();
        SerialMon.println("modem  network is connected");
        // Connect to the GPRS network
        if (!modem.isGprsConnected())
        {
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
                // if GPRS wasn't connected after 2 mins restart esp
                if ((millis() - waitTime) > 120000)
                {
                    sysRestart();
                }
            }
        }
        else
        {
            Serial.println("connected to GPRS");
        }
    }
    else
    {
        SerialMon.println("modem network is not connected");
    }
}
void pinInit()
{
    // define pins mode
    pinMode(valvePin, OUTPUT);
    pinMode(lowHumidityPin, OUTPUT);
    pinMode(normalHumidityPin, OUTPUT);
    pinMode(sim800ResetPin, OUTPUT);
    pinMode(comPin, OUTPUT);
    pinMode(humidityPin, INPUT);

    // initial value
    digitalWrite(valvePin, 0);
    digitalWrite(lowHumidityPin, 0);
    digitalWrite(normalHumidityPin, 0);
    digitalWrite(sim800ResetPin, 1);
    digitalWrite(comPin, 0);
}
bool gsmPost(String postData, String path)
{
    // check modem signal quality
    int signalQuality = modem.getSignalQuality();
    SerialMon.print("Signal quality: ");
    SerialMon.println(signalQuality);

    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        // SerialMon.println("modem network is connected");

        // check if modem gprs is connected
        if (modem.isGprsConnected())
        {
            String postMessage = "info=" + postData;

            // Set the request headers
            http.beginRequest();
            http.post(path);
            http.sendHeader("Content-Type", "application/x-www-form-urlencoded");
            http.sendHeader("Content-Length", postMessage.length());
            // http.sendHeader("X-Custom-Header", "custom-header-value");
            http.beginBody();
            SerialMon.println("");
            SerialMon.println("sent data:");
            SerialMon.println(postMessage);
            SerialMon.println("");

            http.print(postMessage);
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

    int statusCode = 0;
    int tryNum = 0;
    while (statusCode != 200)
    {
        tryNum++;
        // Send the HTTP GET request
        http.get(cmdPath); // Replace with your resource path
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
    deserializeJson(cmdInfo, response);

    // update latest data
    dataUpdate();

    return true;
}

void dataUpdate()
{
    valveCmd = cmdInfo["valveCmd"];
    restartCmd = cmdInfo["restartCmd"];
    durationCmd = cmdInfo["durationCmd"];
    autoIrrEnCmd = cmdInfo["autoIrrEnCmd"];
    howOftenCmd = cmdInfo["howOftenCmd"];
    hourCmd = cmdInfo["hourCmd"];
    minuteCmd = cmdInfo["minuteCmd"];

    humHiLiCmd = cmdInfo["humHiLiCmd"];
    humLoLiCmd = cmdInfo["humLoLiCmd"];

    if (durationCmd != duration || autoIrrEnCmd != autoIrrEn || howOftenCmd != howOften || hourCmd != hour || minuteCmd != minute)
    {
        duration = durationCmd;
        autoIrrEn = autoIrrEnCmd;
        howOften = howOftenCmd;
        hour = hourCmd;
        minute = minuteCmd;

        copy = 1;
        copyTimer = millis();
    }
}
String dataPrepareForSys()
{
    sysInfo["valve"] = valve;
    sysInfo["humidity"] = humidity;
    sysInfo["copy"] = copy;
    sysInfo["duration"] = duration;
    sysInfo["humHiLi"] = humHiLi;
    sysInfo["humLoLi"] = humLoLi;
    sysInfo["lastIrrTS"] = lastIrrTS;
    sysInfo["howOften"] = howOften;
    sysInfo["hour"] = hour;
    sysInfo["minute"] = minute;

    sysInfoJson = "";
    serializeJson(sysInfo, sysInfoJson);

    return sysInfoJson;
}
String dataPrepareForCmd(bool valveSt)
{
    cmdInfo["valveCmd"] = valveSt;
    cmdInfo["restartCmd"] = restart;
    cmdInfo["autoIrrEnCmd"] = autoIrrEn;
    cmdInfo["durationCmd"] = duration;
    cmdInfo["howOftenCmd"] = howOften;
    cmdInfo["hourCmd"] = hour;
    cmdInfo["minuteCmd"] = minute;

    cmdInfo["humHiLiCmd"] = humHiLi;
    cmdInfo["humLoLiCmd"] = humLoLi;

    cmdInfojson = "";
    serializeJson(cmdInfo, cmdInfojson);

    return cmdInfojson;
}
void sysRestart()
{
    digitalWrite(comPin, 1);
    Serial.print("System is gonna restart in 3");
    delay(1000);
    Serial.print(" 2");
    delay(1000);
    Serial.println(" 1");
    digitalWrite(sim800ResetPin, 0);
    delay(1000);
    digitalWrite(sim800ResetPin, 1);
    delay(1000);
    ESP.restart();
}
void blink()
{
    digitalWrite(comPin, 1);
    delay(250);
    digitalWrite(comPin, 0);
    delay(250);
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
bool valveOpen()
{
    Serial.println("Valve Opening...");

    valve = true;
    digitalWrite(valvePin, valve);
    irrStartTime = millis();
    copy = true;
    copyTimer = millis();
    duration = durationCmd;

    lastIrrTS = rtcTimeDate();
    putToEEPROM();
    createNextIrrTimeStamp();

    gsmPost(dataPrepareForCmd(false), cmdPath);
    gsmPost(dataPrepareForSys(), sysPath);
    insertRec();
    return 1;
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
void putToEEPROM()
{
    dataPrepareForSys();
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
int createNextIrrTimeStamp()
{
    nextIrrTS = lastIrrTS + (howOften * 24 * 60 * 60);
    time_t timestamp = nextIrrTS;
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
    nextIrrTS = t;
    return t;
}
bool insertRec()
{
    /*
    1- disconnect mqttClient
    2- perform post req
    3- disconnect httpClient
    4- connect mqtt client again
    */
    // check modem signal quality
    int signalQuality = modem.getSignalQuality();
    SerialMon.print("Signal quality: ");
    SerialMon.println(signalQuality);

    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        // check if modem gprs is connected
        if (modem.isGprsConnected())
        {
            // post request data
            String sendData = "insertIntoDB={\"duration\" : " + String(duration) + "}";
            String path = "/karkevand/php/insertToDb.php";
            String conType = "application/x-www-form-urlencoded";

            Serial.println("inserting new irr record to db");

            Serial.println("sendData: " + sendData);

            // Set the request headers
            http.post(path, conType, sendData);

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
bool valveClose()
{
    Serial.println("Valve Closing...");
    valve = false;
    copy = true;
    copyTimer = millis();
    digitalWrite(valvePin, valve);
    gsmPost(dataPrepareForSys(), sysPath);
    return 1;
};
