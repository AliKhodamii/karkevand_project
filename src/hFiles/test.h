// #define TINY_GSM_MODEM_SIM800  // Define the modem type as SIM800
// #define TINY_GSM_USE_GPRS true // We're using GPRS, not WiFi

// #include <TinyGsmClient.h>
// #include <SoftwareSerial.h>

// // Define the serial connections for SIM800L
// #define SerialMon Serial // For debugging
// #define MODEM_RX D6      // RX pin of ESP8266 connected to TX pin of SIM800L
// #define MODEM_TX D7      // TX pin of ESP8266 connected to RX pin of SIM800L

// // Initialize SoftwareSerial for SIM800L
// SoftwareSerial SerialAT(MODEM_RX, MODEM_TX);
// TinyGsm modem(SerialAT);
// TinyGsmClient client(modem);

// //----------------
// void mqttCallback(char *topic, byte *payload, unsigned int length);
// bool mqttConnect();
// //----------------

// // GPRS credentials
// const char apn[] = "your_apn"; // Replace with your APN
// const char user[] = "";        // GPRS username, if any
// const char pass[] = "";        // GPRS password, if any

// void setup()
// {
//     SerialMon.begin(9600);
//     delay(10);

//     // Start communication with SIM800L at 9600 baud
//     SerialAT.begin(9600);
//     delay(3000);

//     SerialMon.println("Initializing modem...");
//     modem.restart();

//     String modemInfo = modem.getModemInfo();
//     SerialMon.print("Modem Info: ");
//     SerialMon.println(modemInfo);

//     // Wait for network connection
//     while (!modem.waitForNetwork())
//     {
//         SerialMon.println("Connecting to network...");
//         delay(1000);
//     }

//     SerialMon.println("Connected to network!");

//     // Connect to GPRS
//     SerialMon.print("Connecting to GPRS...");
//     if (!modem.gprsConnect(apn, user, pass))
//     {
//         SerialMon.println(" fail");
//         while (true)
//             ;
//     }
//     SerialMon.println(" success");

//     Serial.println(modem.getGSMDateTime(DATE_FULL)); // Print current time

//     // Sync time with NTP
//     if (modem.NTPServerSync())
//     {

//         Serial.println("Time synced successfully");
//         Serial.print("Current time: ");
//         Serial.println(modem.getGSMDateTime(DATE_FULL)); // Print current time
//     }
//     else
//     {
//         Serial.println("Failed to sync time");
//     }
// }

// void loop()
// {
//     String timeString = modem.getGSMDateTime(DATE_FULL);
//     Serial.println(timeString);                           // Print current time
//     int year = timeString.substring(0, 2).toInt() + 2000; // Convert to full year (2024)
//     int month = timeString.substring(3, 5).toInt();
//     int day = timeString.substring(6, 8).toInt();
//     int hour = timeString.substring(9, 11).toInt();
//     int minute = timeString.substring(12, 14).toInt();
//     int second = timeString.substring(15, 17).toInt();
//     int timezoneOffset = timeString.substring(18, 21).toInt(); // Timezone in hours

//     // Print extracted values
//     Serial.print("Year: ");
//     Serial.println(year);
//     Serial.print("Month: ");
//     Serial.println(month);
//     Serial.print("Day: ");
//     Serial.println(day);
//     Serial.print("Hour: ");
//     Serial.println(hour);
//     Serial.print("Minute: ");
//     Serial.println(minute);
//     Serial.print("Second: ");
//     Serial.println(second);
//     Serial.print("Timezone offset: ");
//     Serial.println(timezoneOffset);

//     // Populate the tm structure
//     struct tm tm;
//     tm.tm_year = year - 1900; // Year since 1900
//     tm.tm_mon = month - 1;    // Month (0-11)
//     tm.tm_mday = day;         // Day of the month
//     tm.tm_hour = hour;        // Hour (0-23)
//     tm.tm_min = minute;       // Minutes (0-59)
//     tm.tm_sec = second;       // Seconds (0-59)
//     tm.tm_isdst = 0;          // Daylight saving time flag (not used)

//     // Convert to timestamp (seconds since Jan 1, 1970)
//     time_t t = mktime(&tm);

//     // Print the Unix timestamp
//     Serial.print("Unix Timestamp: ");
//     Serial.println(t);
//     delay(1000);
// }

#include <ESP8266WiFi.h>

void setup()
{
    Serial.begin(9600);
    delay(5000);
    String tmp = "set time to 21:00";
    Serial.println(tmp.indexOf("to"));
    Serial.println(tmp.substring(tmp.indexOf("to") + 2, tmp.indexOf("to") + 4));
    Serial.println(tmp.substring(tmp.indexOf("to") + 5));
}

void loop()
{
}