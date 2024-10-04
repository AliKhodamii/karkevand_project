#define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
#define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi
#define SerialMon Serial       // For debugging
#define MODEM_RX D6            // RX pin of ESP8266 connected to TX pin of SIM800L
#define MODEM_TX D7

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>

// Initialize SoftwareSerial for SIM800L----
SoftwareSerial SerialAT(MODEM_RX, MODEM_TX);

// GPRS credentials-----------
const char apn[] = "rightel"; // Replace with your APN
const char user[] = "";       // GPRS username, if any
const char pass[] = "";       // GPRS password, if any

// MQTT settings
const char *broker = "io.adafruit.com";
const char *mqtt_username = "sedsmarthome";
const char *mqtt_password = "adafruit pass word";
const char *mqtt_topic = "sedsmarthome/feeds/karssg-slash-client";

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

bool mqttConnect();

void setup()
{
    SerialMon.begin(9600); // start serialMon for debug
    delay(10);
    SerialAT.begin(9600); // Start communication with SIM800L at 9600 baud
    delay(3000);

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
            delay(10000);
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

    // MQTT broker setup
    mqtt.setServer(broker, 1883);
    mqtt.setKeepAlive(120);
    // mqtt.setCallback(mqttCallback);
    while (!mqtt.connected())
    {
        // Attempt to connect to MQTT broker
        SerialMon.print("Connecting to MQTT...");
        if (mqttConnect())
        {
            SerialMon.println(" connected");
            if (mqtt.publish(mqtt_topic, "GsmClientTest started"))
            {
                Serial.println("publish good!");
            }
            else
            {
                Serial.println("publish bad!");
            }
        }
        else
        {
            SerialMon.println(" connection failed, retry in 1 sec...");
            Serial.print(mqtt.state());
            delay(1000);
        }
    }
}

void loop()
{

    delay(5000);
    if (mqtt.connected())
    {
        Serial.println("mqtt is connected!");
    }
    else
    {
        Serial.println("mqtt is NOT connected!");
        while (!mqtt.connected())
        {
            Serial.print("reconnect to mqtt...");
            if (!mqttConnect())
            {
                Serial.println(" failed! retry in 1 sec...");
            }
            else
            {
                Serial.println(" succeed!");
            }
            delay(1000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    String sendMessage;
    String recJson = "";
    // blink when message arrived
    Serial.println("---------------------------");
    Serial.println("");
    Serial.println("");

    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, length);
    SerialMon.println();

    Serial.println("");
    Serial.println("");
    Serial.println("---------------------------");
}
bool mqttConnect()
{
    return mqtt.connect("karSSG", mqtt_username, mqtt_password); // You can add MQTT username/password if required
}
