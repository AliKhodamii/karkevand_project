// #include <ESP8266WiFi.h>
// #include <SoftwareSerial.h>
// #include <ArduinoJson.h>

// // initialize serial pins
// SoftwareSerial SIM800serial(D6, D7); // RX = D6 TX = D7

// // some define
// #define OK 1
// #define NOTOK 2
// #define TIMEOUT 3

// String txt;
// String AT_reply;
// String httpResponse = "";

// int loopcnt = 0;

// // json variable
// JsonDocument info1;   // the info we get from server stores in this
// String postData = ""; // the json we send stores here

// const String host = "http://sed-smarthome.ir/karkevand"; // host

// String SIM800read();
// byte SIM800waitFor(String response1, String response2, uint16_t timeOut);
// byte SIM800command(String command, String response1, String response2, uint16_t timeOut, uint16_t repetitions);
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
// }

// void loop()
// {
//     loopcnt++;
//     Serial.print("start loop: ");
//     Serial.println(loopcnt);
//     String response = "";
//     response = simGet();
//     Serial.println("response = " + response);
//     delay(15000);

//     //   String info_json;
//     //   info_json = simGet();
//     //   Serial.print("response = ");
//     //   Serial.println(info_json);

//     //   if(info_json != "ERROR"){
//     //      deserializeJson(info1, info_json);
//     //      int wk = info1["working time"];
//     //      bool valve = info1["valve"];
//     //      Serial.println(wk);
//     //      Serial.println(valve);
//     // }
//     // else{
//     //   Serial.println("couldn't get the info_json;");
//     // }
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
//         AT_reply = reply;
//     }

//     return reply;
// }

// byte SIM800waitFor(String response1, String response2, uint16_t timeOut)
// {
//     uint16_t entry = 0;
//     // uint16_t count = 0;
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
//     String response = "";
//     bool ok = true;

//     Serial.println();
//     Serial.println("start get request");

//     // check connection to sim800 with command "at"
//     if (ok)
//     {
//         Serial.println("sending AT");
//         SIM800command("AT", "OK", "ERROR", 2000, 1);

//         // check if connection is ok
//         Serial.println("AT_reply = " + AT_reply);
//         if (AT_reply.indexOf("OK") < 0)
//         {
//             ok = false;
//             Serial.println("ERROR!!! AT reply was not OK");
//             return "error";
//         }
//     }

//     // check connection to network with "AT+CREG"
//     if (ok)
//     {
//         Serial.println("sending CREG to know if network is registered");
//         SIM800command("AT+CREG?", "OK", "ERROR", 2000, 1);

//         // check if network is registered
//         Serial.println("AT_reply = " + AT_reply);
//         if (AT_reply.indexOf("+CREG: 0,1") < 0)
//         {
//             ok = false;
//             Serial.println("ERROR!!! network isn't registered");
//             return "error";
//         }
//     }

//     // check connection quality with "AT+CSQ"
//     if (ok)
//     {
//         Serial.println("sending CSQ to check connection quality");
//         SIM800command("AT+CSQ", "OK", "ERROR", 2000, 1);
//         Serial.println("AT_reply = " + AT_reply);
//     }

//     return response;
// }
// // SIM800command("AT+SAPBR=3,1,\"APN\",\"mtnirancell\"", "OK", "ERROR", 2000, 1);                                  // Set bearer parameters
// // SIM800command("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", "ERROR", 2000, 1);                                     // Set bearer parameters
// // SIM800command("AT+SAPBR=1,1", "OK", "ERROR", 5000, 1);                                                          // To open a GPRS context
// // SIM800command("AT+SAPBR=2,1", "OK", "ERROR", 5000, 1);                                                          /// To query the GPRS context
// // SIM800command("AT+HTTPINIT", "OK", "ERROR", 30000, 1);                                                          // Initialize HTTP Service
// // SIM800command("AT+HTTPPARA=\"URL\",\"sed-smarthome.ir/karkevand/information.php\"", "OK", "ERROR", 20000, 1); // HTTP client URL
// // SIM800command("AT+HTTPACTION=0", "0,200,", "ERROR", 20000, 1);                                                  // HTTP Method 0-GET; 1-POST; 2-HEAD
// // SIM800command("AT+HTTPREAD", "HTTPREAD", "ERROR", 20000, 1);                                                    // Read the HTTP Server Response

// // response = txt;
// // if (response != "ERROR")
// // {
// //   response.remove(0, response.indexOf("{"));
// // }
// // else
// // {
// //   Serial.println("ERROR" + response);
// // }

// // SIM800command("AT+HTTPTERM", "HTTPTERM", "ERROR", 20000, 1); // Terminate HTTP Service
// // SIM800command("AT+SAPBR=0,1", "+SAPBR:", "ERROR", 2000, 1);  // Close bearer

// // return response;
// // }
