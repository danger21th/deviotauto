#include "FS.h"
#include "LittleFS.h"
#include "mywifi.h"
#include <ArduinoJson.h>

void saveWifi()
{
    StaticJsonDocument<1800> docmqtt;
    docmqtt["set_ssid"] = set_ssid;
    docmqtt["set_password"] = set_password;

    LittleFS.remove("/wifi.txt");
    configFile = LittleFS.open("/wifi.txt", "w+");
    String output = "";
    serializeJson(docmqtt, output);
    Serial.println(output);
    configFile.println(output);
    configFile.close();
}

void readfileWifi()
{
    String tmp = String(set_ssid) + ":" + String(set_password);
    Serial.println("readfileWifi");
    Serial.println(tmp);

    configFile = LittleFS.open("/wifi.txt", "r");

    if (!configFile)
    {
        Serial.println("Failed to open file for reading");
        configFile = LittleFS.open("/wifi.txt", "w+");
        Serial.println("create file system /wifi.txt");
        //configFile.println(tmp);
        configFile.close();
        //return;
        configFile = LittleFS.open("/wifi.txt", "r");
        if (!configFile)
        {
            Serial.println("Failed to open file for reading");
        }
    }

    Serial.println("File Content:");
    String acess_mqtt = "";
    while (configFile.available())
    {
        char c = configFile.read();
        Serial.write(c);
        acess_mqtt += char(c);
    }
    //Serial.println("acess_mqtt=" + acess_mqtt);
    configFile.close();

    set_ssid = "";
    set_password = "";

    StaticJsonDocument<1800> docmqtt;
    deserializeJson(docmqtt, acess_mqtt);
    String output = "";
    serializeJson(docmqtt["set_ssid"], set_ssid);
    set_ssid.replace("\"", "");
    ssid = set_ssid;

    serializeJson(docmqtt["set_password"], set_password);
    set_password.replace("\"", "");
    password = set_password;
}