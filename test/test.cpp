// #include <ESP8266WiFi.h>
// // #include <ESP8266HTTPClient.h>
// // #include <WiFiClient.h>
// // #include <ESP8266WebServer.h>
// // #include <ArduinoJson.h>
// // #include <TimeLib.h>

// // const char *ssid = "TP_Sed";
// // const char *password = "87654321";
// // const String host = "http://sed-smarthome.ir/SmartGarden/";
// // const int analogPin = A0;
// // WiFiServer server(80);
// // HTTPClient http;
// // WiFiClient wifi;
// // long long unsigned int startTime = 0, ledTime = millis();

// // int checkTime = millis(), duration = 5,
// //     shir = 16,
// //     checkLed = 0,
// //     ledState = 0, rotubat = 0, copy = 0, copyTimer = 0, clinetTimer = 0, rotubat_array_index = 0;
// // int rotubat_sample_count = 10, rotubat_array[10];
// // String restartESP = "no", serverValue = "", Valve = "off", setInfo = "";
// // DynamicJsonDocument serverData(1024);

// // /*---------------------------------------------*/
// // /*---------------------------------------------*/
// // /*-----------------FUNCTIONS-------------------*/
// // /*---------------------------------------------*/
// // /*---------------------------------------------*/

// // int get_rotubat()
// // {
// //   if (analogRead(analogPin) != 0)
// //   {
// //     if (rotubat_array_index > rotubat_sample_count - 1)
// //     {
// //       rotubat_array_index = 0;
// //     }
// //     rotubat_array[rotubat_array_index] = analogRead(analogPin);
// //     rotubat_array_index++;
// //     int i = 0;
// //     int sum = 0;
// //     while ((rotubat_array[i] != NULL) && i <= rotubat_sample_count - 1)
// //     {
// //       sum = sum + rotubat_array[i];
// //       i++;
// //       if (i == rotubat_sample_count)
// //         break;
// //     }
// //     int average = sum / i;
// //     return average;
// //   }
// //   else
// //   {
// //     return 0;
// //   }
// // }

// // /// @brief
// // /// @param
// // /// @return
// // int mypost(void)
// // {
// //   rotubat = get_rotubat();
// //   http.begin(wifi, host + "poster.php");
// //   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
// //   //  setInfo = "millis = " + String(millis()) + "\nvalveValue = " + Valve + "&restarter = " + restartESP + "&rotubat = " + String(rotubat) + "&duration = " + String(duration) + "\ncopy = " + String(copy);
// //   serverData["time passed since last restart"] = String(day() - 1) + "d :" + String(hour()) + "h :" + String(minute()) + "m : " + String(second()) + "s ";
// //   serverData["valveValue"] = Valve;
// //   serverData["restarter"] = restartESP;
// //   serverData["rotubat"] = rotubat;
// //   serverData["duration"] = duration;
// //   serverData["copy"] = copy;
// //   setInfo = "";
// //   serializeJson(serverData, setInfo);

// //   auto httpCode = http.POST("info=" + setInfo);
// //   http.end(); // Close connection Serial.println();
// //   //  Serial.println("rotubat = " + String(rotubat));
// //   return httpCode;
// // }

// // void myget(void)
// // {
// //   http.begin(wifi, host + "information.php");
// //   int httpResponseCode = http.GET();
// //   String getRespond = "";
// //   delay(100);
// //   getRespond = http.getString();
// //   http.end();
// //   if (httpResponseCode >= 0 && getRespond != "-1" && getRespond != "-11")
// //     deserializeJson(serverData, getRespond);
// // }

// // void myValveOpen()
// // {
// //   digitalWrite(shir, 1);
// //   Valve = "on";
// //   copy = 1;
// //   copyTimer = millis();
// //   mypost();
// //   Serial.println("Set Valve_value => on");
// //   startTime = millis();
// // }
// // void myValveClose()
// // {
// //   digitalWrite(shir, 0);
// //   Valve = "off";
// //   copy = 1;
// //   copyTimer = millis();
// //   mypost();
// //   Serial.println("Set Valve_value => off");
// //   startTime = 0;
// // }
// // // String time () {
// // //   val = DateTime.now();
// // //   int days = elapsedDays(val);
// // //   int hours = numberOfHours(val);
// // //   int minutes = numberOfMinutes(val);
// // //   int seconds = numberOfSeconds(val);
// // //   return (String(days) + "d : " + String(hours) + "h :" + String(minutes) + "m :" + String(seconds) + "s");
// // // }

// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/
// // /*START*/

// // void setup()
// // {
// //   Serial.begin(9600);
// //   delay(100);
// //   pinMode(shir, OUTPUT);
// //   pinMode(checkLed, OUTPUT);
// //   digitalWrite(shir, 0);
// //   digitalWrite(checkLed, ledState);
// //   Serial.println();
// //   Serial.println();
// //   Serial.print("Connecting to ");
// //   Serial.println(ssid);
// //   WiFi.begin(ssid, password);
// //   while (WiFi.status() != WL_CONNECTED)
// //   {
// //     delay(250);
// //     Serial.print(".");
// //   }
// //   Serial.println("");
// //   Serial.println("WiFi Connected.");
// //   server.begin();
// //   Serial.println("Server Started");
// //   Serial.println(WiFi.localIP());

// //   myget();
// //   rotubat = analogRead(analogPin);
// //   duration = serverData["duration"];
// //   //  duration = serverdata.substring(serverdata.indexOf("duration =") + 11 , serverdata.indexOf("copy =") - 1).toInt();
// //   mypost();
// // }
// // void loop()
// // {
// //   // age etesal ba modem ghat beshe cheskmak taghir mikone va kolan dige kari nmikone
// //   if (WiFi.status() != WL_CONNECTED)
// //   {
// //     WiFi.begin(ssid, password);
// //     while (WiFi.status() != WL_CONNECTED)
// //     {
// //       delay(250);
// //       Serial.print(".");
// //       if (ledState == 0)
// //       {
// //         ledState = 1;
// //       }
// //       else
// //       {
// //         ledState = 0;
// //       }
// //       digitalWrite(checkLed, ledState);
// //     }
// //   }

// //   /* har 5 sanie meghdar millis mire ro server
// //      meghdar shir dar server daryaft mishe
// //      meghdar reset daryaft mieshe
// //      age taghirati bayad etefagh biofte etefagh miofte*/
// //   if (millis() - checkTime > 5000)
// //   {
// //     checkTime = millis();
// //     // daryafte etelaat az server

// //     myget();
// //     serverValue = String(serverData["valveValue"]);
// //     restartESP = String(serverData["restarter"]);
// //     duration = serverData["duration"];
// //     Serial.println("serverValue =" + serverValue + "\nrestartESP = " + restartESP + "\nduration = " + String(duration));

// //     // baz kardane shir az server
// //     if (serverValue.indexOf("on") != -1 && Valve == "off")
// //     {
// //       Serial.println("open from server");
// //       myValveOpen();
// //     }

// //     // bastane shir az server
// //     if (serverValue.indexOf("off") != -1 && Valve == "on")
// //     {
// //       Serial.println("from site");
// //       myValveClose();
// //     }

// //     // bastane shir ba zaman
// //     if ((millis() - startTime) > (duration * 60 * 1000) && Valve == "on")
// //     {
// //       myValveClose();
// //       Serial.println("form duration");
// //     }

// //     int javab = mypost();
// //     Serial.println("http post respond: " + String(javab));

// //     ledState = 1;
// //     digitalWrite(checkLed, ledState);
// //     delay(100);
// //     ledState = 0;
// //     digitalWrite(checkLed, ledState);
// //     //    Serial.println(setInfo);
// //   }
// //   // check the copy number
// //   if ((millis() - copyTimer > 10000) && (copy == 1))
// //   {
// //     copy = 0;
// //     mypost();
// //   }

// //   // reset bad az 1 saat va ya ba dastoor az server
// //   if (restartESP == "yes")
// //   {
// //     ESP.reset();
// //   }
// // }
