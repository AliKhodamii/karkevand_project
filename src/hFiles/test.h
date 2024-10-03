#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include<PubSubClient.h>

// Wi-Fi credentials
const char *ssid = "TP_Sed";       // Replace with your Wi-Fi SSID
const char *password = "87654321"; // Replace with your Wi-Fi password

// Server details
const char *serverUrl = "http://sed-smarthome.ir/karkevand/php/insertToDb.php"; // Replace with your server URL

void setup()
{
    Serial.begin(9600);

    // Connect to Wi-Fi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // Wait for the connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Only attempt HTTP POST if connected to Wi-Fi
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http; // Create HTTP client object
        WiFiClient client;
        PubSubClient mqtt(client);

        // Start connection and send HTTP POST request
        Serial.println("Starting POST request...");
        http.begin(client, serverUrl);                // Specify the destination for the HTTP request
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Specify content-type header

        // Data to send (in JSON format)
        String postData = "insertIntoDB={\"duration\":66}";
        Serial.println("postData: " + postData);

        // Send the request and receive the response code
        int httpResponseCode = http.POST(postData);

        // Check for a valid response
        if (httpResponseCode > 0)
        {
            String responseBody = http.getString(); // Get the response from the server
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            Serial.print("Response body: ");
            Serial.println(responseBody);
        }
        else
        {
            // If an error occurred
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        // Close the connection
        http.end();
    }
    else
    {
        Serial.println("Error: Not connected to WiFi");
    }
}

void loop()
{
    // Nothing needed here for now
}
