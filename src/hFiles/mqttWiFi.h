#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi and MQTT server details
const char *ssid = "Sed_HS";
const char *password = "Qwer4321";
const char *mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi();

void setup()
{
    Serial.begin(9600);

    // Connect to WiFi
    setup_wifi();

    // Set up MQTT server
    client.setServer(mqtt_server, 1883);

    // Connect to MQTT broker
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...");
        if (client.connect("karSSG"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            delay(5000);
        }
    }

    // \"workingTime\":\"0d :0h :0m :3s\",\"valve\":false,\"restart\":false,\"copy\":false,\"humidity\":17,
    // Prepare the message to publish
    String msg = "{\"time\":\"10:10:10:13\",\"valve\":false,\"restart\":false,\"humidity\":17,\"duration\":111,\"humHiLi\":0111,\"humLoLi\":1110,\"lastIrrTS\":1727427421,\"autoIrrTS\":tru1e,\"howOften\":11,\"hour\":81,\"minute\":57}";
    String message = "valve is close, info" + msg;
    char msgBuffer[500];                 // Adjust buffer size based on expected message length
    message.toCharArray(msgBuffer, 500); // Convert String to char array
    Serial.println(msgBuffer);

    // Publish the message
    Serial.println(client.publish("karSSG/ESP", msgBuffer));
}

void loop()
{
    client.loop();
}

void setup_wifi()
{
    delay(10);
    // Connecting to WiFi
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
}
