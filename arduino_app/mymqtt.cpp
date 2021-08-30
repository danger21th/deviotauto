
#include "FS.h"
#include "LittleFS.h"
//#include "FS.h"
#include "mymqtt.h"
#include <ArduinoJson.h>

String acess_ya29 = "";

void saveMQtt()
{
    StaticJsonDocument<1800> docmqtt;
    docmqtt["mqtt_server"] = mqttServer;
    docmqtt["mqtt_port"] = mqttPort;
    docmqtt["mqtt_usrname"] = mqttUser;
    docmqtt["mqtt_passwd"] = mqttPassword;
    docmqtt["client_id"] = CLIENT_ID;
    docmqtt["mqtttopic"] = mqtttopic;
    docmqtt["mqttget"] = mqttget;

    LittleFS.remove("/mqtt.txt");
    configFile = LittleFS.open("/mqtt.txt", "w+");
    String output = "";
    serializeJson(docmqtt, output);
    Serial.println(output);
    configFile.println(output);
    configFile.close();
}

void readfileMqtt()
{
    String tmp = String(mqttServer) + ":" + String(mqttUser) + ":" + String(mqttPassword) + ":" + String(CLIENT_ID) + ":";
    tmp = tmp + ":" + String(mqttPort);
    Serial.println(tmp);

    File file = LittleFS.open("/mqtt.txt", "r");

    if (!file)
    {
        Serial.println("Failed to open file for reading");
        configFile = LittleFS.open("/mqtt.txt", "w+");
        Serial.println("create file system /mqtt.txt");
        //configFile.println(tmp);
        configFile.close();
        return;
    }
    else
    {
    }
    Serial.println("File Content:");
    String acess_mqtt = "";
    while (file.available())
    {
        char c = file.read();
        Serial.write(c);
        acess_mqtt += char(c);
    }
    //Serial.println("acess_mqtt=" + acess_mqtt);
    file.close();

    mqttServer = "";
    mqttUser = "";
    mqttPassword = "";
    CLIENT_ID = "";
    mqtttopic = "";
    mqttget = "";

    StaticJsonDocument<1800> docmqtt;
    deserializeJson(docmqtt, acess_mqtt);
    String output = "";
    serializeJson(docmqtt["mqtt_server"], mqttServer);
    mqttServer.replace("\"", "");
    serializeJson(docmqtt["mqtt_port"], output);
    output.replace("\"", "");
    mqttPort = output.toInt();
    serializeJson(docmqtt["mqtt_usrname"], mqttUser);
    mqttUser.replace("\"", "");
    serializeJson(docmqtt["mqtt_passwd"], mqttPassword);
    mqttPassword.replace("\"", "");
    serializeJson(docmqtt["client_id"], CLIENT_ID);
    CLIENT_ID.replace("\"", "");
    serializeJson(docmqtt["mqtttopic"], mqtttopic);
    mqtttopic.replace("\"", "");
    serializeJson(docmqtt["mqttget"], mqttget);
    mqttget.replace("\"", "");
    //serializeJson(doc, output);
    //output.replace("\"", "");
    //Serial.println(output);
}