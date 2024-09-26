// #include <ESP8266WiFi.h>
// #include <SoftwareSerial.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>
// #include <ESP8266WebServer.h>
// #include <ArduinoJson.h>
// #include <TimeLib.h>
// #include <NTPClient.h>

// // initialize serial pins
// SoftwareSerial SIM800serial(D6, D7);

// // some define
// #define OK 1
// #define NOTOK 2
// #define TIMEOUT 3

// // pin define
// const int garden1_valve_pin = 10; // valve 1
// const int garden2_valve_pin = 11; // valve 2
// const int soil1 = 12;             // soil moisture senseor 1
// const int soil2 = 13;             // soil moisture senseor 2
// const int tx = 4;                 // tx for communication to sim800
// const int rx = 3;                 // rx for communication to sim800
// const int connection_signal = 9;  // a pin to show connection state

// // local variables
// bool garden1_valve_state = false; // local valve 1
// bool garden2_valve_state = false; // local valve 2
// int garden1_duration = 0;
// int garden2_duration = 0;
// int garden1_howOften = 0;
// int garden2_howOften = 0;
// int garden1_last_irrigation = 0;
// int garden2_last_irrigation = 0;
// String txt;
// String httpResponse = "";

// // json variable
// DynamicJsonDocument info(1024); // the info we get from server stores in this
// String postData = "";           // the json we send stores here

// // time variables
// unsigned long preTime = 0;
// long garden1_startTime = 0;
// long garden2_startTime = 0;
// long nowTimeStamp = 0;
// long garden1_copyTime;
// long garden2_copyTime;

// const String host = "https://sed-smarthome.ir/karkevand"; // host

// void garden1_valve_open();
// void garden2_valve_open();
// void garden1_valve_close();
// void garden2_valve_close();
// String SIM800read();
// byte SIM800waitFor();
// byte SIM800command();
// String simGet();

// void setup()
// {
//     // Serial begin
//     Serial.begin(9600);

//     // sim800 serial start
//     SIM800serial.begin(9600);
//     delay(100);
//     // first at command
//     SIM800command("AT", "OK", "ERROR", 500, 5);

//     // pin initialize
//     pinMode(garden1_valve_pin, OUTPUT);
//     pinMode(garden2_valve_pin, OUTPUT);
//     pinMode(soil1, INPUT);
//     pinMode(soil2, INPUT);
//     pinMode(connection_signal, OUTPUT);
//     // internet connection

//     // get data from server
//     // update duration and howOften values

//     // initialize primary values
//     digitalWrite(garden1_valve_pin, LOW);
//     digitalWrite(garden2_valve_pin, LOW);
//     digitalWrite(connection_signal, LOW);

//     // post data to server

//     //
// }

// void loop()
// {

//     // check if net is still connected

//     // DO EVERY 5 seconds
//     if (millis() - preTime > 5000)
//     {
//         preTime = millis();
//         // get data from server

//         // check if any command has been sent

//         if (info["garden1"]["valve"] && !garden1_valve_state) // valve 1 open command
//         {
//             Serial.println("garden1_valve is opening - server command");
//             garden1_valve_open();
//         }
//         if (info["garden2"]["valve"] && !garden2_valve_state) // valve 2 open command
//         {
//             Serial.println("garden2_valve is opening - server command");
//             garden2_valve_open();
//         }
//         if (!info["garden1"]["valve"] && garden1_valve_state) // valve 1 close command
//         {

//             Serial.println("garden1_valve is closing... - server command");
//             garden1_valve_close();
//         }
//         if (!info["garden2"]["valve"] && garden2_valve_state) // valve 2 close command
//         {
//             Serial.println("garden2_valve is closing... - server command");
//             garden2_valve_close();
//         }

//         // check if irrigation time is finished
//         if (garden1_valve_state && ((millis() - garden1_startTime) > (garden1_duration * 60 * 1000)))
//         {
//             Serial.println("garden1_valve is closing... - time out");
//             garden1_valve_close();
//         }
//         if (garden2_valve_state && ((millis() - garden2_startTime) > (garden2_duration * 60 * 1000)))
//         {
//             Serial.println("garden2_valve is closing... - time out");
//             garden2_valve_close();
//         }

//         // check if it"s time for schedule irrigation

//         if (info["garden1"]["schedule_irrigation"])
//         {
//             if (nowTimeStamp - garden1_last_irrigation > (garden1_howOften * 24 * 60 * 60))
//             {
//                 Serial.println("garden1_valve is opening... - schedule irrigation");
//                 garden1_valve_open();
//             }
//         }
//         if (info["garden1"]["schedule_irrigation"])
//         {
//             if (nowTimeStamp - garden2_last_irrigation > (garden2_howOften * 24 * 60 * 60))

//             {
//                 Serial.println("garden2_valve is opening... - schedule irrigation");
//                 garden2_valve_open();
//             }
//         }

//         // post new data to server
//     }

//     // check if copy time has been ended
//     if (millis() - garden1_copyTime > 10 * 1000)
//     {
//         Serial.println("garden1_copy changed to false - time out");
//         info["garden1"]["copy"] = false;
//         // post
//     }
//     if (millis() - garden2_copyTime > 10 * 1000)
//     {
//         Serial.println("garden2_copy changed to false - time out");
//         info["garden2"]["copy"] = false;
//         // post
//     }
// }

// void garden1_valve_open()
// {
//     digitalWrite(garden1_valve_pin, HIGH);
//     garden1_valve_state = true;
//     info["garden1"]["valve"] = true;
//     garden1_startTime = millis();
//     info["garden1"]["last_irrigation"] = 0; // timestamp will be here
//     info["garden1"]["copy"] = true;

//     garden1_copyTime = millis();

//     Serial.println();
//     Serial.println("garden1_valve is opened");

//     Serial.println();
//     Serial.println("changes :");
//     Serial.println("garden1_valve_state = true");
//     Serial.println("garde1_copy = true");
//     Serial.println("garden1_startTime = millis()");
//     Serial.println("garden1_last_irrigation updated");

//     Serial.println();
//     Serial.println("posting data after garden1_valve open");
//     // post
//     // insert record into DB
// }
// void garden2_valve_open()
// {
//     digitalWrite(garden2_valve_pin, HIGH);
//     garden2_valve_state = true;
//     info["garden2"]["valve"] = true;
//     garden2_startTime = millis();
//     info["garden2"]["last_irrigation"] = 0; // the timestamp will be here
//     info["garden2"]["copy"] = true;

//     garden1_copyTime = millis();

//     Serial.println();
//     Serial.println("garden2_valve is opened");

//     Serial.println();
//     Serial.println("changes :");
//     Serial.println("garden2_valve_state = true");
//     Serial.println("garde2_copy = true");
//     Serial.println("garden2_startTime = millis()");
//     Serial.println("garden2_last_irrigation updated");

//     Serial.println();
//     Serial.println("posting data after garden2_valve open");

//     // post
//     // insert record into DB
// }
// void garden1_valve_close()
// {
//     digitalWrite(garden1_valve_pin, LOW);
//     garden1_valve_state = false;
//     info["garden1"]["valve"] = false;
//     info["garden1"]["copy"] = true;

//     garden1_copyTime = millis();

//     Serial.println();
//     Serial.println("garden1_valve is closed");

//     Serial.println();
//     Serial.println("changes :");
//     Serial.println("garden1_valve_state = false");
//     Serial.println("garde1_copy = true");

//     Serial.println();
//     Serial.println("posting data after garden1_valve close");

//     // post
// }
// void garden2_valve_close()
// {
//     digitalWrite(garden2_valve_pin, LOW);
//     garden2_valve_state = false;
//     info["garden2"]["valve"] = false;
//     info["garden2"]["copy"] = true;

//     garden1_copyTime = millis();

//     Serial.println();
//     Serial.println("garden1_valve is closed");

//     Serial.println();
//     Serial.println("changes :");
//     Serial.println("garden1_valve_state = false");
//     Serial.println("garde1_copy = true");

//     Serial.println();
//     Serial.println("posting data after garden1_valve close");

//     // post
// }

// String SIM800read()
// {
//     String reply = "";

//     if (SIM800serial.available())
//     {
//         reply = SIM800serial.readString();
//     }

//     if (reply != "")
//     {
//         Serial.print("Reply: ");
//         Serial.println(reply);
//         txt = reply;
//     }

//     return reply;
// }

// byte SIM800waitFor(String response1, String response2, uint16_t timeOut)
// {
//     uint16_t entry = 0;
//     uint16_t count = 0;
//     String reply = SIM800read();
//     byte retVal = 99;

//     do
//     {
//         reply = SIM800read();
//         delay(1);
//         entry++;
//     } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && entry < timeOut);

//     if (entry >= timeOut)
//     {
//         retVal = TIMEOUT;
//     }
//     else
//     {
//         if (reply.indexOf(response1) + reply.indexOf(response2) > -2)
//             retVal = OK;
//         else
//             retVal = NOTOK;
//     }

//     return retVal;
// }

// byte SIM800command(String command, String response1, String response2, uint16_t timeOut, uint16_t repetitions)
// {
//     byte returnValue = NOTOK;
//     byte countt = 0;

//     while (countt < repetitions && returnValue != OK)
//     {
//         SIM800serial.println(command);

//         if (SIM800waitFor(response1, response2, timeOut) == OK)
//         {
//             returnValue = OK;
//         }
//         else
//         {
//             returnValue = NOTOK;
//         }
//         countt++;
//     }

//     return returnValue;
// }
// String simGet()
// {
//     SIM800command("AT+SAPBR=3,1,\"APN\",\"mtnirancell\"", "OK", "ERROR", 2000, 1);                                  // Set bearer parameters
//     SIM800command("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", "ERROR", 2000, 1);                                     // Set bearer parameters
//     SIM800command("AT+SAPBR=1,1", "OK", "ERROR", 5000, 1);                                                          // To open a GPRS context
//     SIM800command("AT+SAPBR=2,1", "OK", "ERROR", 5000, 1);                                                          /// To query the GPRS context
//     SIM800command("AT+HTTPINIT", "OK", "ERROR", 30000, 1);                                                          // Initialize HTTP Service
//     SIM800command("AT+HTTPPARA=\"URL\",\"sed-smarthome.ir/SmartGarden/information.txt\"", "OK", "ERROR", 20000, 1); // HTTP client URL
//     SIM800command("AT+HTTPACTION=0", "0,200,", "ERROR", 20000, 1);                                                  // HTTP Method 0-GET; 1-POST; 2-HEAD
//     SIM800command("AT+HTTPREAD", "HTTPREAD", "ERROR", 20000, 1);                                                    // Read the HTTP Server Response
//     txt.remove(0, txt.indexOf("{"));
//     Serial.print("txt = ");
//     Serial.println(txt);
//     SIM800command("AT+HTTPTERM", "HTTPTERM", "ERROR", 20000, 1); // Terminate HTTP Service
//     SIM800command("AT+SAPBR=0,1", "+SAPBR:", "ERROR", 2000, 1);  // Close bearer
// }
