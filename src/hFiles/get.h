
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define rxPin D6
#define txPin D7
SoftwareSerial sim800L(rxPin, txPin);

int loop_number = 0;

bool GPRS_init();
String GPRS_get();
void GPRS_trim();
String sendAT(String command);

void setup()
{
    // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
    Serial.begin(9600);
    Serial.println("Setup");
    // Begin serial communication with Arduino and SIM800L
    sim800L.begin(9600);

    while (!GPRS_init())
    {
        Serial.println("GPRS_init");
        delay(10000);
    };
}

void loop()
{
    while (sim800L.available())
    {
        Serial.println(sim800L.readString());
    }
    while (Serial.available())
    {
        sim800L.println(Serial.readString());
    }
    // Serial.println("-----------------------------------------------------------");
    // loop_number++;
    // Serial.print("Loop Number: ");
    // Serial.println(loop_number);

    delay(10000);
    GPRS_get();
    // for (int i = 0; i < 5; i++)
    // {
    //   if (GPRS_get() != "ERROR")
    //   {
    //     break;
    //   }
    //   delay(5000);
    // }
    // delay(5000);
    // String tmp;
    // tmp = sendAT("AT");
    // Serial.println("tmp = " + tmp);
    // delay(5000);
    // tmp = sendAT("AT+CSQ");
    // Serial.println("tmp = " + tmp);
    // delay(5000);
    // tmp = sendAT("AT+CREG?");
    // Serial.println("tmp = " + tmp);
}

bool GPRS_init()
{
    bool ok = 1;
    String response = "";

    // Check if Sim800 is connected
    response = sendAT("AT");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("gprs_init was unsuccessful, AT command has bad response");
        ok = 0;
        return ok;
    }

    // check signal strength
    response = sendAT("AT+CSQ");
    int space = response.indexOf(" ");
    int comma = response.indexOf(",");
    int signal_strength = (response.substring(space + 1, comma)).toInt();
    if (signal_strength < 20)
    {
        Serial.println("gprs_init was unsuccessful, signal strength is not good");
        ok = 0;
        return ok;
    }

    // check connection to network
    response = sendAT("AT+CREG?");
    if (response.indexOf("+CREG: 0,1") == -1)
    {
        Serial.println("gprs_init was unsuccessful, sim wasn't registered");
        ok = 0;
        return ok;
    }

    // set Contype to GPRS
    response = sendAT("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("gprs_init was unsuccessful, couldn't set Contype to GPRS");
        ok = 0;
        return ok;
    }

    // set APN to irancell
    response = sendAT("AT+SAPBR=3,1,\"APN\",\"mtnirancell\"");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("gprs_init was unsuccessful, could't set the APN");
        ok = 0;
        return ok;
    }

    // open sapbr for to start connection to network
    response = sendAT("AT+SAPBR=1,1");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("gprs_init was unsuccessful, couldn't turn on sapbr");
        // ok = 0;
        // return ok;
    }

    // check sapbr status and get ip
    response = sendAT("AT+SAPBR=2,1");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("gprs_init was unsuccessful, couldn't get ip");
        // ok = 0;
        // return ok;
    }

    return ok;
}

String GPRS_get()
{
    String ret = "ERROR";
    String response = "";
    int sapbr_stat;

    // sim800L.println("AT+SAPBR=2,1");
    // delay(100);
    // while (sim800L.available())
    // {
    //   response = sim800L.readString();
    //   Serial.println(response);
    //   int first_comma = response.indexOf(",");
    //   int second_comma = response.indexOf(",", first_comma + 1);
    //   int third_comma = response.indexOf(",", second_comma + 1);
    //   sapbr_stat = (response.substring(second_comma + 1, third_comma)).toInt();
    // }
    // Serial.println(sapbr_stat);

    // if (sapbr_stat == 3)
    // {

    //   sim800L.println("AT+SAPBR=1,1");
    //   delay(2000);
    //   while (sim800L.available())
    //   {
    //     response = (sim800L.readString());
    //     Serial.println(response);
    //     // check for error
    //     if (response.indexOf("OK") == -1)
    //     {
    //       Serial.println("GPRS_GET was not successful, sapbr couldn't be set");
    //       GPRS_trim();
    //       ret = "ERROR";
    //       return ret;
    //     }
    //   }
    // }

    // initial http
    response = sendAT("AT+HTTPINIT");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("GPRS_GET was not successful, http couldn't be initialised");
        GPRS_trim();
        ret = "ERROR";
        return ret;
    }

    // set url
    response = sendAT("AT+HTTPPARA=\"URL\",\"sed-smarthome.ir/SmartGarden/information.php\"");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("GPRS_GET was not successful, URL couldn't be set");
        GPRS_trim();
        ret = "ERROR";
        return ret;
    }

    // set http request type to get
    response = sendAT("AT+HTTPACTION = 0");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("GPRS_GET was not successful, http action wan not successful");
        GPRS_trim();
        ret = "ERROR";
        return ret;
    }

    // wait for answer
    int max_try = 10;
    while (response.indexOf("+HTTPACTION:") < 0)
    {
        delay(2000);
        while (sim800L.available())
        {
            response = (sim800L.readString());
            Serial.println("res = " + response);
        }

        max_try--;
        if (!max_try)
        {
            Serial.println("GPRS_GET was not successful, http answer timed out");
            GPRS_trim();
            ret = "ERROR";
            return ret;
        }
    }

    // find out what the http response code was
    int get_req_res;
    int first_comma = response.indexOf(",");
    int second_comma = response.indexOf(",", first_comma + 1);
    get_req_res = (response.substring(first_comma + 1, second_comma)).toInt();

    // read http
    response = sendAT("AT+HTTPREAD");
    if (response.indexOf("OK") == -1)
    {
        Serial.println("GPRS_GET was not successful, http read was not successful");
        GPRS_trim();
        ret = "ERROR";
        return ret;
    }

    // while (sim800L.available())
    // {
    //   Serial.println(sim800L.readString());
    //   response = (sim800L.readString());

    // extract json from sim800 response
    if (get_req_res == 200)
    {
        int first_bracket = response.indexOf("{");
        int second_bracket = response.indexOf("}");

        ret = response.substring(first_bracket, second_bracket + 1);
        Serial.println("return = ");
        Serial.println(ret);
    }
    // }

    // terminate http
    response = sendAT("AT+HTTPTERM");

    // sim800L.println("AT+SAPBR=0,1");
    // delay(2000);
    // while (sim800L.available())
    // {
    //   Serial.println(sim800L.readString());
    // }
    // sim800L.println("AT+SAPBR=0,1");
    // delay(2000);
    // while (sim800L.available())
    // {
    //   Serial.println(sim800L.readString());
    // }
    return ret;
}

void GPRS_trim()
{
    sim800L.flush();
    sim800L.println("AT+HTTPTERM");
    delay(100);
    while (sim800L.available())
    {
        Serial.println(sim800L.readString());
    }
}

String sendAT(String command)
{
    String response = "";
    sim800L.println(command);
    delay(100);
    while (1)
    {
        while (sim800L.available())
        {
            response = sim800L.readString();
            Serial.println(response);
        }
        return response;
    }
    return response;
}
