// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>
// #include <PubSubClient.h>

// void callback(char* topic , byte* payload , unsigned int length);
// void connect();

// // Wi-Fi credentials
// const char *ssid = "TP_Sed";       // Replace with your Wi-Fi SSID
// const char *password = "87654321"; // Replace with your Wi-Fi password

// // Server details
// const char *serverUrl = "http://sed-smarthome.ir/karkevand/php/insertToDb.php"; // Replace with your server URL

// WiFiClient client;
// PubSubClient mqtt(client);

// const char *broker = "test.mosquitto.org";
// const int port = 1883;
// const char *pubTopic = "sedSSG/ESP";
// const char *subTopic = "sedSSG/client";

// void setup()
// {
//     Serial.begin(9600);

//     // Connect to Wi-Fi
//     Serial.print("Connecting to ");
//     Serial.println(ssid);
//     WiFi.begin(ssid, password);

//     // Wait for the connection
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(1000);
//         Serial.print(".");
//     }

//     Serial.println();
//     Serial.println("WiFi connected");
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());

//     // Only attempt HTTP POST if connected to Wi-Fi
//     if (WiFi.status() == WL_CONNECTED)
//     {

//         mqtt.setServer(broker, port);
//         mqtt.setCallback(callback);
//     }
//     else
//     {
//         Serial.println("Error: Not connected to WiFi");
//     }
// }

// void loop()
// {
//     if (!mqtt.connected())
//     {
//         connect();
//     }
//     mqtt.loop();
// }

// void connect()
// {
//     while (!mqtt.connected())
//     {
//         Serial.print("Connecting to MQTT...");
//         String mqttClientId = "sedEsp";
//         if (mqtt.connect(mqttClientId.c_str()))
//         {
//             Serial.println(" success!");
//             mqtt.subscribe(subTopic);
//         }
//         else
//         {
//             Serial.print(" Fail! rc=");
//             Serial.println(mqtt.state());
//             delay(5000);
//         }
//     }
// }

// void callback(char *topic, byte *payload, unsigned int length)
// {
//     Serial.print("Message arrived with topic: ");
//     Serial.println(topic);
//     Serial.println("with payload :");
//     String message = "";
//     for (int i = 0; i < length; i++)
//     {
//         message += (char)payload[i];
//         Serial.print((char)payload[i]);
//     }
//     Serial.println();

//     if (message == "hi")
//     {
//         mqtt.publish(pubTopic, "hello");
//     }
// }

#include <ESP8266WiFi.h>

void setup()
{
    pinMode(D1, OUTPUT);
    digitalWrite(D1, LOW);
}
void loop() {}