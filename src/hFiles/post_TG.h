#define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
#define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//-------
String data = "";
//------

// Define the serial connections for SIM800L
#define SerialMon Serial // For debugging
#define MODEM_RX D6      // RX pin of ESP8266 connected to TX pin of SIM800L
#define MODEM_TX D7      // TX pin of ESP8266 connected to RX pin of SIM800L

// Initialize SoftwareSerial for SIM800L
SoftwareSerial SerialAT(MODEM_RX, MODEM_TX);

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, "sed-smarthome.ir", 80); // Replace with your server address

// GPRS credentials
const char apn[] = "rightel"; // Replace with your APN
const char user[] = "";       // GPRS username, if any
const char pass[] = "";       // GPRS password, if any

void setup()
{
    SerialMon.begin(9600);
    delay(10);

    SerialAT.begin(9600); // Start communication with SIM800L at 9600 baud
    delay(3000);

    //------------------------------------
    JsonDocument doc;

    doc["time"] = 4564;
    doc["valve"] = true;

    serializeJson(doc, data);
    //------------------------------------

    // Restart the modem to establish communication
    modem.restart();

    // Print modem information
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        SerialMon.println("mode network is connected");
        // Connect to the GPRS network
        SerialMon.print("Connecting to the network...");
        if (!modem.gprsConnect(apn, user, pass))
        {
            SerialMon.println(" fail");
            while (true)
                ;
        }
        SerialMon.println(" success");
    }
    else
    {
        SerialMon.println("mode network is not connected");
        while (true)
            ;
    }
}




void loop()
{
    // check modem signal quality
    int signalQuality = modem.getSignalQuality();
    SerialMon.print("Signal quality: ");
    SerialMon.println(signalQuality);

    // check if modem is connected to network
    if (modem.isNetworkConnected())
    {
        Serial.println("modem network is connected");

        // check if modem gprs is connected
        if (modem.isGprsConnected())
        {
            Serial.println("modem GPRS is connected");

            // connect to server
            if (!client.connected())
            {
                SerialMon.println("Connecting to server...");
                if (!client.connect("sed-smarthome.ir", 80))
                { // Replace with your server address
                    SerialMon.println(" fail");
                    delay(10000);
                    return;
                }
                SerialMon.println(" success");
            }

            // Set the request headers
            http.beginRequest();
            http.post("/karkevand/poster.php");
            http.sendHeader("Content-Type", "application/x-www-form-urlencoded");
            http.sendHeader("Content-Length", data.length() + 5);
            // http.sendHeader("X-Custom-Header", "custom-header-value");
            http.beginBody();
            Serial.println("");
            Serial.println("sent data:");
            Serial.println(data);
            Serial.println("");

            http.print("info=" + data);
            http.endRequest();
            SerialMon.println("POST request sent");

            // Read the response
            int statusCode = http.responseStatusCode();
            String response = http.responseBody();

            Serial.print("Status code: ");
            Serial.println(statusCode);
            Serial.print("Response: ");
            Serial.println(response);
            // Close the connection
            http.stop();
            SerialMon.println("Server disconnected");
        }
        else
        {
            Serial.println("modem GPRS is not connected");

            // try 2 time to connect modem GPRS
            for (int i = 1; i <= 2; i++)
            {
                Serial.print("trying to connect modem GPRS, try number: ");
                Serial.println(i);
                // Connect to the GPRS network
                SerialMon.print("Connecting to the network...");
                if (modem.gprsConnect(apn, user, pass))
                {
                    SerialMon.println(" success");
                    return;
                }
                SerialMon.println(" fail");
            }
            SerialMon.println("unable to connect modem GPRS");
        }
    }
    else
    {
        Serial.println("modem network is not connected");
    }

    // Delay before making another request
    delay(10000);
}

bool postInit(){
    
}
