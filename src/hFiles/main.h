// defines--------------------
#define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
#define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi
#define SerialMon Serial       // For debugging
#define MODEM_RX D7            // RX pin of ESP8266 connected to TX pin of SIM800L
#define MODEM_TX D6            // TX pin of ESP8266 connected to RX pin of SIM800L
// ---------------------------

// includes-------------------
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
// ---------------------------

// variables------------------
const char *serverPath = "sed-smarthome.ir";
const String getPath = "/karkevand/httpProt/getInfo.php";
const String postPath = "/karkevand/httpProt/postInfo.php";
String sysInfoJson = "";

// GPRS credentials
const char apn[] = "rightel"; // Replace with your APN
const char user[] = "";       // GPRS username, if any
const char pass[] = "";       // GPRS password, if any

// control variables
bool valve = false;
bool s_valve = false;
bool restart = false;
bool copy = false;
int humidity = 0;
int duration = 0;
int humidityHighLimit = 0;
int humidityLowLimit = 0;
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

// Initialize SoftwareSerial for SIM800L
SoftwareSerial SerialAT(MODEM_RX, MODEM_TX);

// init modem and http client
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, serverPath, 80); // Replace with your server address

// function forward declaration------
bool gsmPost();
bool gsmGet();
void dataUpdate();
void dataPrepare();
//-----------------------------------

// declare json handler
JsonDocument sysInfo;

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

    pinMode(humidityPin, INPUT);

    // pins initial values
    digitalWrite(valvePin, 0);
    digitalWrite(lowHumidityPin, 0);
    digitalWrite(normalHumidityPin, 0);
    digitalWrite(sim800ResetPin, 1);
    digitalWrite(comPin, 0);

    // if sim800 isn't registered we will wait
    if (!modem.isNetworkConnected())
    {
        while (!modem.isNetworkConnected())
        {
            // start modem (gsm module)
            modem.restart();

            // Print modem information
            String modemInfo = modem.getModemInfo();
            SerialMon.print("Modem Info: ");
            SerialMon.println(modemInfo);
            Serial.println("modem is not connected to network, waiting 5 seconds...");
            delay(5000);
        }
    }
    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        SerialMon.println("modem  network is connected");
        // Connect to the GPRS network
        if (!modem.isGprsConnected())
        {
            while (!modem.gprsConnect(apn, user, pass))
            {
                SerialMon.print("Connecting to the GPRS...");
                if (!modem.gprsConnect(apn, user, pass))
                {
                    SerialMon.println(" fail");
                }
                SerialMon.println(" success");
            }
        }
    }
    else
    {
        SerialMon.println("modem network is not connected");
    }

    // first http get request to get initial values
    while (!gsmGet())
        delay(1000);
    // initial values
    valve = false;
    restart = false;
    copy = false;
    // first http post request to post inital values (like valve = false)
    while (!gsmPost())
        delay(1000);
}
void loop()
{
    // check if sim800 is connected to network
    while (!modem.isNetworkConnected())
    {
        SerialMon.println("modem is not connected to network");
        delay(5000);
    }

    // connect to GPRS if we're not
    while (modem.isNetworkConnected() && !modem.isGprsConnected())
    {
        SerialMon.println("modem is not connected to GPRS");
        SerialMon.print("Connecting to the GPRS network...");
        if (!modem.gprsConnect(apn, user, pass))
        {
            SerialMon.println(" fail");
        }
        SerialMon.println(" success");
    }
    Serial.println("");
    Serial.println("");
    Serial.println("Start get request");
    gsmGet();
    Serial.println("End get request");
    delay(3000);
    Serial.println("");
    Serial.println("");
    Serial.println("Start post request");
    gsmPost();
    Serial.println("End post request");

    delay(5000);
}

bool gsmPost()
{
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

            // // connect to server
            // if (!client.connected())
            // {
            //     SerialMon.println("Connecting to server...");
            //     if (!client.connect(serverPath, 80))
            //     { // Replace with your server address
            //         SerialMon.println(" fail");
            //         return false;
            //     }
            //     SerialMon.println(" success");
            // }

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

    // if (!client.connected())
    // {
    //     SerialMon.println("Connecting to server...");
    //     if (!client.connect(serverPath, 80))
    //     { // Replace with your server address
    //         SerialMon.println(" fail");
    //         delay(10000);
    //         return "ERROR";
    //     }
    //     SerialMon.println(" success");
    // }

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
}
void dataPrepare()
{
    sysInfo["valve"] = valve;
    sysInfo["restart"] = restart;
    sysInfo["humidity"] = humidity;
    sysInfo["copy"] = copy;
    sysInfo["duration"] = duration;
    sysInfo["humidityHighLimit"] = humidityHighLimit;
    sysInfo["humidityLowLimit"] = humidityLowLimit;

    sysInfoJson = "";
    serializeJson(sysInfo, sysInfoJson);
}
