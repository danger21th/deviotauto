#include "FS.h"
#include "LittleFS.h"
//#include "FS.h"
#include "timeschedule.h"
#include <ArduinoJson.h>

void savetime()
{
    Serial.println(String(_timeschedule1) + ":" + String(_timeschedule2) + ":" + String(_timeschedule3) + ":" + String(_timeschedule4) + ":" + String(_timeschedule5) + ":" + String(_timeschedule6) + ":");
    LittleFS.remove("/timeschedule.txt");
    configFile = LittleFS.open("/timeschedule.txt", "w+");
    configFile.println(String(_timeschedule1) + ":" + String(_timeschedule2) + ":" + String(_timeschedule3) + ":" + String(_timeschedule4) + ":" + String(_timeschedule5) + ":" + String(_timeschedule6) + ":");
    configFile.close();
}

void savetime_sw(String bnt)
{
    Serial.println("savetime_sw");
    StaticJsonDocument<1200> docmqtt;
    docmqtt["1"] = _timeschedule1;
    docmqtt["2"] = _timeschedule2;
    docmqtt["3"] = _timeschedule3;
    docmqtt["4"] = _timeschedule4;
    docmqtt["5"] = _timeschedule5;
    docmqtt["6"] = _timeschedule6;
    docmqtt["7"] = _timeschedule7;
    docmqtt["8"] = _timeschedule8;
    LittleFS.remove("/timeschedule_" + bnt + ".txt");
    configFile = LittleFS.open("/timeschedule_" + bnt + ".txt", "w+");
    String output = "";
    serializeJson(docmqtt, output);
    Serial.println(output);
    configFile.println(output);
    docmqtt.clear();
    configFile.close();
}
void readfileTime_sw(String bnt)
{
    Serial.println("readfileTime_sw" + bnt);
    configFile = LittleFS.open("/timeschedule_" + bnt + ".txt", "r");

    if (!configFile)
    {
        Serial.println("Failed to open file for reading");
        configFile = LittleFS.open("/timeschedule_" + bnt + ".txt", "w+");
        Serial.println("create file system /timeschedule_" + bnt + ".txt");
        //configFile.println(tmp);
        configFile.close();

        return;
    }
    else
    {
    }
    Serial.println("File Content:/timeschedule_" + bnt + ".txt");
    String acess_mqtt = "";
    while (configFile.available())
    {

        char c = configFile.read();
        Serial.write(c);
        acess_mqtt += char(c);
    }

    _timeschedule1 = "";
    _timeschedule2 = "";
    _timeschedule3 = "";
    _timeschedule4 = "";
    _timeschedule5 = "";
    _timeschedule6 = "";
    _timeschedule7 = "";
    _timeschedule8 = "";
    //StaticJsonDocument<2200> docmqtt;
    DynamicJsonDocument docmqtt(2200);
    deserializeJson(docmqtt, acess_mqtt);
    String output = "";
    serializeJson(docmqtt["1"], _timeschedule1);
    _timeschedule1.replace("\"", "");
    serializeJson(docmqtt["2"], _timeschedule2);
    _timeschedule2.replace("\"", "");
    serializeJson(docmqtt["3"], _timeschedule3);
    _timeschedule3.replace("\"", "");
    serializeJson(docmqtt["4"], _timeschedule4);
    _timeschedule4.replace("\"", "");
    serializeJson(docmqtt["5"], _timeschedule5);
    _timeschedule5.replace("\"", "");
    serializeJson(docmqtt["6"], _timeschedule6);
    _timeschedule6.replace("\"", "");
    serializeJson(docmqtt["7"], _timeschedule7);
    _timeschedule7.replace("\"", "");
    serializeJson(docmqtt["8"], _timeschedule8);
    _timeschedule8.replace("\"", "");
    docmqtt.clear();
    configFile.close();
}

void readfileTime()
{
    File file = LittleFS.open("/timeschedule.txt", "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        configFile = LittleFS.open("/timeschedule.txt", "w+");
        Serial.println("create file system /timeschedule.txt");
        configFile.println(String(_timeschedule1) + ":" + String(_timeschedule2) + ":" + String(_timeschedule3) + ":" + String(_timeschedule4) + ":");
        configFile.close();
        return;
    }
    Serial.println("File Content:");
    String acess_ya29 = "";
    while (file.available())
    {
        char c = file.read();
        Serial.write(c);
        acess_ya29 += char(c);
    }
    Serial.println("acess_ya29=" + acess_ya29);
    file.close();

    _timeschedule1 = getValue(acess_ya29, ':', 0);
    _timeschedule2 = getValue(acess_ya29, ':', 1);
    _timeschedule3 = getValue(acess_ya29, ':', 2);
    _timeschedule4 = getValue(acess_ya29, ':', 3);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    int count = 0;
    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        count++;
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
        if (count > 100)
        {
            break;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}