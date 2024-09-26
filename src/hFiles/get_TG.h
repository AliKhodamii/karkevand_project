#define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
#define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>

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

    // Restart the modem to establish communication
    modem.restart();

    // Print modem information
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

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

void loop()
{
    int signalQuality = modem.getSignalQuality();
    SerialMon.print("Signal quality: ");
    SerialMon.println(signalQuality);

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

    // Send the HTTP GET request
    http.get("/SmartGarden/information.txt"); // Replace with your resource path

    // Read the response status code
    int statusCode = http.responseStatusCode();
    SerialMon.print("Response status code: ");
    SerialMon.println(statusCode);

    // Read the response body
    String response = http.responseBody();
    SerialMon.println("Response:");
    SerialMon.println(response);

    // Close the connection
    http.stop();
    SerialMon.println("Server disconnected");

    // Delay before making another request
    delay(10000);
}
