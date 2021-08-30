#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include "LittleFS.h" //https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/
//https://www.lazada.co.th/products/i2069946739-s6758312205.html
//https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
//mcp3008
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include "mymqtt.h"
#include "mywifi.h"
#include "timeschedule.h"

#include <WiFiManager.h>
#include <DNSServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


unsigned long Mqtt_lastMs = 0;
unsigned int cnt = 0;
char msg[1000];
//https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/
String ssid = "xxxxxxxxx"; 
String password = "xxxxxx";     
String page = "";

File configFile;

int WiFiCount = 0;
int config_done = 0;

IPAddress ip(192, 168, 1, 211);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(1, 1, 1, 1); //IPAddress dns1(8, 8, 8, 8);
IPAddress dns2(1, 0, 0, 1); //IPAddress dns2(8, 8, 4, 4); (9, 9, 9,9)
ESP8266WebServer server(80);

//IPAddress mqttServer(192, 168, 1, 38);
String mqttServer = "192.168.1.38";
String mqttUser = "u";
String mqttPassword = "p";
String CLIENT_ID = "local.esp12f";
int mqttPort = 1883;
String mqtttopic = "/homeswitch/user/update";
String mqttget = "/homeswitch/user/get";

String set_ssid = "";
String set_password = "";

void handleRoot();
void handleNotFound();
void led1on();
void led1off();

void led2on();
void led2off();

void led3on();
void led3off();

void led4on();
void led4off();

void savestatus();
//void savetime();
//void readfileTime();
//String getValue(String data, char separator, int index);
String switch4();
String switch4time(String sw);

void jsoncmd();
void onoff();
void timeswitch();
void wifiform();
void switchtime();
void reset_switchtime();

void set_wifi();
void mqttcontrol();
void software_Reboot();
StaticJsonDocument<512> paratojson(String _s, String i1, String i2, String i3, String i4, String i5, String i6);

const int relay1 = 5; //GPIO5
const int relay2 = 4; //GPIO4
const int relay3 = 0; //GPIO0
const int relay4 = 2; //GPIO2

String _s1 = "OFF";
String _s2 = "OFF";
String _s3 = "OFF";
String _s4 = "OFF";

//{"1":"12_10_ON_START_S1_","2":"12_12_OFF_STOP_S1_","3":"15_12_ON_STOP_S2_","4":"15_20_OFF_STOP_S2_","5":"15_20_OFF_STOP_S2_","6":"15_20_OFF_STOP_S3_"}
String _timeschedule1 = "12_10_OFF_START_S1_";
String _timeschedule2 = "12_12_OFF_STOP_S1_";
String _timeschedule3 = "15_12_OFF_STOP_S2_";
String _timeschedule4 = "15_20_OFF_STOP_S2_";
String _timeschedule5 = "15_20_OFF_STOP_S3_";
String _timeschedule6 = "15_20_OFF_STOP_S3_";
String _timeschedule7 = "15_20_OFF_STOP_S3_";
String _timeschedule8 = "15_20_OFF_STOP_S3_";

//https://randomnerdtutorials.com/esp32-esp8266-web-server-outputs-momentary-switch/
//https://randomnerdtutorials.com/esp8266-web-server-with-arduino-ide/
// HTML web page
WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<h2>4switch<h2>
<h3> Wifi Set ESP8266</h3>

<form action="/set_wifi">
  set_ssid:<br>
  <input type="text" name="set_ssid" value="">
  <br>
  set_password:<br>
  <input type="text" name="set_password" value="">
  <br><br>
  <input type="submit" value="Submit">
</form> 

</body>
</html>
)=====";

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';

  String command_flutter = (char *)payload;
  //StaticJsonDocument<1800> docflutter;
  DynamicJsonDocument docflutter(1800);
  deserializeJson(docflutter, command_flutter);
  String output = "";
  String sw = "";
  String data = "";
  String page = "";
  String command = "";
  String action = "";
  String t = "";
  serializeJson(docflutter["id"], output);
  serializeJson(docflutter["sw"], sw);
  serializeJson(docflutter["data"], data);
  serializeJson(docflutter["page"], page);
  serializeJson(docflutter["command"], command);
  serializeJson(docflutter["action"], action);
  serializeJson(docflutter["t"], t);
  Serial.println();
  Serial.println("id=" + String(output));
  output.replace("\"", "");
  sw.replace("\"", "");
  data.replace("\"", "");
  page.replace("\"", "");
  command.replace("\"", "");
  action.replace("\"", "");
  t.replace("\"", "");
  Serial.println((char *)payload);
  command_flutter.clear();
  if (sw == "1")
  {
    if (data == "ON")
    {
      led1on();
    }
    else if (data == "OFF")
    {
      led1off();
    }
  }
  else if (sw == "2")
  {
    if (data == "ON")
    {
      led2on();
    }
    else if (data == "OFF")
    {
      led2off();
    }
  }
  else if (sw == "3")
  {
    if (data == "ON")
    {
      led3on();
    }
    else if (data == "OFF")
    {
      led3off();
    }
  }
  else if (sw == "4")
  {
    if (data == "ON")
    {
      led4on();
    }
    else if (data == "OFF")
    {
      led4off();
    }
  }

  if (page == "server_detail")
  {
    snprintf(msg, 1000, "{\"id\":\"%s\",\"mqtt\":\"ok\",\"p\":\"server_detail\"}", output.c_str());
    client.publish(mqtttopic.c_str(), msg);
  }
  else if (page == "server_status")
  {
    if (data == "listswitch")
    {
      Serial.println("listswitch");
      String output = switch4();
      Serial.println(output);
      //output = "{switch4()}";

      output.toCharArray(msg, (output.length() + 1));
      client.publish(mqtttopic.c_str(), msg);
    }
    else if (data == "ON" || data == "OFF")
    {
      String output = switch4();
      output.toCharArray(msg, (output.length() + 1));
      client.publish(mqtttopic.c_str(), msg);
    }
    else
    {
      snprintf(msg, 1000, "{\"id\":\"%s\",\"mqtt\":\"ok\",\"p\":\"server_status\"}", output.c_str());
      client.publish(mqtttopic.c_str(), msg);
    }
  }
  else if (page == "server_status_time")
  {
    Serial.println("server_status_time");
    if (action == "time")
    {
      Serial.println("time");
      readfileTime_sw(sw);

      if (t == "1")
      {
        _timeschedule1 = command;
      }
      else if (t == "2")
      {
        _timeschedule2 = command;
      }
      else if (t == "3")
      {
        _timeschedule3 = command;
      }
      else if (t == "4")
      {
        _timeschedule4 = command;
      }
      else if (t == "5")
      {
        _timeschedule5 = command;
      }
      else if (t == "6")
      {
        _timeschedule6 = command;
      }
      else if (t == "7")
      {
        _timeschedule7 = command;
      }
      else if (t == "8")
      {
        _timeschedule8 = command;
      }
      savetime_sw(sw);

      String output = switch4time(sw);
      output.toCharArray(msg, (output.length() + 1));
      client.publish(mqtttopic.c_str(), msg);
    }
    else
    {
      Serial.println("no time");
      readfileTime_sw(sw);
      String output = switch4time(sw);
      output.toCharArray(msg, (output.length() + 1));
      client.publish(mqtttopic.c_str(), msg);
    }
  }
  else
  {
    snprintf(msg, 1000, "{\"id\":\"%s\",\"mqtt\":\"ok\"}", output.c_str());
    client.publish(mqtttopic.c_str(), msg);
  }

  //{id:"1629470248442",sw:"1",data:"plus",page:"server_status"}
  //{id:"1629474687836",sw:"1",data:"ON",page:"server_detail"}
}

void mqttCheckConnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT Server ...");
    Serial.println(CLIENT_ID);
    Serial.println(mqttUser);
    Serial.println(mqttPassword);
    //client.setInsecure();
    //if (client.connect(CLIENT_ID))
    //WiFi.mode(WIFI_STA); WiFi.mode(WIFI_AP_STA);
    //if (client.connect(CLIENT_ID, MQTT_USRNAME, MQTT_PASSWD))
    if (client.connect(CLIENT_ID.c_str(), mqttUser.c_str(), mqttPassword.c_str()))
    {
      Serial.println("MQTT Connected!");
      Serial.println(mqttget);
      boolean d = client.subscribe(mqttget.c_str());
      Serial.println(d);
    }
    else
    {
      Serial.print("MQTT Connect err:");
      Serial.println(client.state());
      delay(1000);
    }
  }
}

void mqttIntervalPost()
{
  //char param[32];
  //char jsonBuf[128];
  /*
    sprintf(param, "{\"idle\":%d}", digitalRead(13));
    sprintf(jsonBuf, ALINK_BODY_FORMAT, param);
    Serial.println(jsonBuf);
    boolean d = client.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
    Serial.print("publish:0 Failure;1 Success");
    Serial.println(d);
    */
  cnt = cnt + 1; //++cnt;
  char _cs1[4] = "";
  _s1.toCharArray(_cs1, 4);
  char _cs2[4] = "";
  _s2.toCharArray(_cs2, 4);
  char _cs3[4] = "";
  _s3.toCharArray(_cs3, 4);
  char _cs4[4] = "";
  _s4.toCharArray(_cs4, 4);

  snprintf(msg, 1000, "{\"id\":\"%05d\",\"sw1\":\"%s\",\"sw2\":\"%s\",\"sw3\":\"%s\",\"sw4\":\"%s\",\"p\":\"Post\"}", cnt, _cs1, _cs2, _cs3, _cs4);
  Serial.print("Publish message: ");
  Serial.println(mqtttopic);
  Serial.println(msg);
  boolean d = client.publish(mqtttopic.c_str(), msg);
  Serial.print("publish:1 Failure;0  Success");
  Serial.println(d);
  Serial.println(cnt);
  // Serial.println("-------------");

  ////////////////////////
}

void setup()
{
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay3, HIGH);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay4, HIGH);

  Serial.begin(115200);
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  readfileWifi();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("esp4switch", "deviotauto");
  WiFi.begin(ssid.c_str(), password.c_str());
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  //WiFi.beginSmartConfig();
  while ((WiFi.status() != WL_CONNECTED) && (WiFiCount < 20))
  {
    delay(500);
    Serial.print(".");
    WiFiCount++;
  }
  if (WiFiCount >= 20)
  {

    set_ssid = "COMFAST_pharaphi"; //COMFAST_pharaphi AndroidAP
    set_password = "123456ab";     //123456ab 12345678
    saveWifi();
    software_Reboot();
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

  server.on("/", handleRoot);
  server.on("/led1on", led1on);
  server.on("/led1off", led1off);
  server.on("/led2on", led2on);
  server.on("/led2off", led2off);
  server.on("/led3on", led3on);
  server.on("/led3off", led3off);
  server.on("/led4on", led4on);
  server.on("/led4off", led4off);

  server.on("/onoff", onoff);
  server.on("/time", timeswitch);
  server.on("/mqtt", mqttcontrol);
  server.on("/jsoncmd", jsoncmd);
  server.on("/switchtime", switchtime); //switchtime?sw=
  server.on("/reset_switchtime", reset_switchtime);
  server.on("/wifiform", wifiform);
  server.on("/set_wifi", set_wifi);
  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });
  server.onNotFound(handleNotFound);

  server.begin();

  File file = LittleFS.open("/switch.txt", "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    configFile = LittleFS.open("/switch.txt", "w+");
    Serial.println("create file system /switch.txt");
    configFile.println(String(_s1) + ":" + String(_s2) + ":" + String(_s3) + ":" + String(_s4) + ":");
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

  _s1 = getValue(acess_ya29, ':', 0);
  _s2 = getValue(acess_ya29, ':', 1);
  _s3 = getValue(acess_ya29, ':', 2);
  _s4 = getValue(acess_ya29, ':', 3);
  _s4.replace("\"", "");
  _s4.replace("\n", "");
  _s4.replace(" ", "");
  (_s1 == "ON") ? digitalWrite(relay1, LOW) : digitalWrite(relay1, HIGH);
  (_s2 == "ON") ? digitalWrite(relay2, LOW) : digitalWrite(relay2, HIGH);
  (_s3 == "ON") ? digitalWrite(relay3, LOW) : digitalWrite(relay3, HIGH);
  (_s4 == "ON") ? digitalWrite(relay4, LOW) : digitalWrite(relay4, HIGH);

  readfileTime_sw("1");
  readfileTime_sw("2");
  readfileTime_sw("3");
  readfileTime_sw("4");
  //readfileTime();
  readfileMqtt();
  //readfileWifi();
  Serial.print("espClient [");

  //Serial.println(MQTT_SERVER);
  //client.setServer(MQTT_SERVER, MQTT_PORT);
  /* Connect to the MQTT server after connecting to the Wi-Fi */
  Serial.println(mqttServer); //MQTT_SERVER //mqttServer
  client.setServer(mqttServer.c_str(), mqttPort);
  client.setCallback(callback);
  Serial.print("]");
}

void loop() // Code under this loop runs forever.
{
  server.handleClient();

  if (millis() - Mqtt_lastMs >= 30000) //10000
  {
    Mqtt_lastMs = millis();
    mqttCheckConnect();
    /*The heartbeat interval */
    //cnt = cnt + 1;
    mqttIntervalPost();
  }
  client.loop();
}
void set_wifi()
{
  String message = "";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "set_ssid")
    {
      set_ssid = server.arg(i);
    }
    if (server.argName(i) == "set_password")
    {
      set_password = server.arg(i);
    }
  }
  saveWifi();
  //server.send(200, "application/json", "{\"command\":\"wifisave\"}");
  server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"1; URL=/\" />");
}
void wifiform()
{
  String s = MAIN_page;             //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

void led1off()
{
  _s1 = "OFF";
  digitalWrite(relay1, HIGH);
  Serial.println("1 Current not Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led1on()
{
  _s1 = "ON";
  digitalWrite(relay1, LOW);
  Serial.println("1 Current Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led2off()
{
  _s2 = "OFF";
  digitalWrite(relay2, HIGH);
  Serial.println("2 Current not Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led2on()
{
  _s2 = "ON";
  digitalWrite(relay2, LOW);
  Serial.println("2 Current Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led3off()
{
  _s3 = "OFF";
  digitalWrite(relay3, HIGH);
  Serial.println("3 Current not Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led3on()
{
  _s3 = "ON";
  digitalWrite(relay3, LOW);
  Serial.println("3 Current Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led4off()
{
  _s4 = "OFF";
  digitalWrite(relay4, HIGH);
  Serial.println("4 Current not Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void led4on()
{
  _s4 = "ON";
  digitalWrite(relay4, LOW);
  Serial.println("4 Current Flowing");
  //server.send(200, "application/json", "{\"command\":\"ok\"}");
  savestatus();
}

void savestatus()
{
  Serial.println(String(_s1) + ":" + String(_s2) + ":" + String(_s3) + ":" + String(_s4) + ":");
  LittleFS.remove("/switch.txt");
  configFile = LittleFS.open("/switch.txt", "w+");
  configFile.println(String(_s1) + ":" + String(_s2) + ":" + String(_s3) + ":" + String(_s4) + ":");
  configFile.close();
  if (page == "server_status")
  {
    page = "";
    server.send(200, "application/json", "");
  }
  else if (page == "servercontrol")
  {
    page = "";
    server.send(200, "application/json", "{\"command\":\"button\" ,\"status\":\"ok\"}");
  }
  else
  {
    server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"1; URL=/\" />");
  }
}

void mqttcontrol()
{
  Serial.println("mqttcontrol");
  String message = "";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "mqtt_server")
    {
      mqttServer = server.arg(i);
    }
    if (server.argName(i) == "client_id")
    {
      //CLIENT_ID = server.arg(i);
      CLIENT_ID = WiFi.macAddress();
    }
    if (server.argName(i) == "mqtt_port")
    {
      mqttPort = server.arg(i).toInt();
    }
    if (server.argName(i) == "mqtt_usrname")
    {
      mqttUser = server.arg(i);
    }
    if (server.argName(i) == "mqtt_passwd")
    {
      mqttPassword = server.arg(i);
    }
    if (server.argName(i) == "mqtttopic")
    {
      mqtttopic = server.arg(i);
    }
    if (server.argName(i) == "mqttget")
    {
      mqttget = server.arg(i);
    }
  }
  saveMQtt();
  server.send(200, "application/json", "{\"command\":\"mqtt\" ,\"status\":\"ok\"}");
  software_Reboot();
}

void timeswitch()
{
  String message = "";
  String t = "";
  String v = "";
  String sw = "";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "t")
    {
      t = server.arg(i);
    }
    if (server.argName(i) == "v")
    {
      v = server.arg(i);
    }
    if (server.argName(i) == "sw")
    {
      sw = server.arg(i);
    }
  }
  readfileTime_sw(sw);

  if (t == "1")
  {
    _timeschedule1 = v;
  }
  if (t == "2")
  {
    _timeschedule2 = v;
  }
  if (t == "3")
  {
    _timeschedule3 = v;
  }
  if (t == "4")
  {
    _timeschedule4 = v;
  }
  if (t == "5")
  {
    _timeschedule5 = v;
  }
  if (t == "6")
  {
    _timeschedule6 = v;
  }
  if (t == "7")
  {
    _timeschedule7 = v;
  }
  if (t == "8")
  {
    _timeschedule8 = v;
  }
  //savetime();

  savetime_sw(sw);
  Serial.println(message);
  server.send(200, "application/json", "{\"command\":\"ok\"}");
}

void handleRoot()
{
  String header = "<head>"
                  "<meta http-equiv=\"refresh\" content=\"5\">"
                  "</head>";

  String html_s1 = (_s1 == "OFF") ? "<a href=\"/led1on\">/change</a><br>" : "<a href=\"/led1off\">/change</a><br>";
  String html_s2 = (_s2 == "OFF") ? "<a href=\"/led2on\">/change</a><br>" : "<a href=\"/led2off\">/change</a><br>";
  String html_s3 = (_s3 == "OFF") ? "<a href=\"/led3on\">/change</a><br>" : "<a href=\"/led3off\">/change</a><br>";
  String html_s4 = (_s4 == "OFF") ? "<a href=\"/led4on\">/change</a><br>" : "<a href=\"/led4off\">/change</a><br>";
  String temp = header + "esp 4swtich<br>" + _s1 + html_s1 + _s2 + html_s2 + _s3 + html_s3 + _s4 + html_s4;
  temp = temp + "<a href=\"/wifiform\">/wifiform</a><br>";

  /*
  String temp = "esp 4swtich<br>" \               
             "<a href=\"/led1on\">/led1on</a><br>" \
             "<a href=\"/led1off\">/led1off</a><br>" \   
                "<a href=\"/led2on\">/led2on</a><br>" \  
                "<a href=\"/led2off\">/led2off</a><br>" \ 
                "<a href=\"/led3on\">/led3on</a><br>"
                "<a href=\"/led3off\">/led3off</a><br>"
                "<br>" \ 
                "<a href=\"/reset\">/reset</a><br>";
  */

  server.send(200, "text/html", temp);
}

void onoff()
{
  String message = "";
  String s = "";
  String status = "";
  String doit = "";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "s")
    {
      s = server.arg(i);
    }
    if (server.argName(i) == "status")
    {
      status = server.arg(i);
    }
    if (server.argName(i) == "doit")
    {
      doit = server.arg(i);
    }
    if (server.argName(i) == "page")
    {
      page = server.arg(i);
    }
  }

  if (doit != "")
  {
    if (s == "1")
    {
      if (doit == "ON")
      {
        led1on();
      }
      else
      {
        led1off();
      }
    }
    else if (s == "2")
    {
      if (doit == "ON")
      {
        led2on();
      }
      else
      {
        led2off();
      }
    }
    else if (s == "3")
    {
      if (doit == "ON")
      {
        led3on();
      }
      else
      {
        led3off();
      }
    }
    else if (s == "4")
    {
      if (doit == "ON")
      {
        led4on();
      }
      else
      {
        led4off();
      }
    }
  }
  else
  {
    if (s == "1")
    {
      if (status == "ON")
      {
        led1on();
      }
      else
      {
        led1off();
      }
    }
    else if (s == "2")
    {
      if (status == "ON")
      {
        led2on();
      }
      else
      {
        led2off();
      }
    }
    else if (s == "3")
    {
      if (status == "ON")
      {
        led3on();
      }
      else
      {
        led3off();
      }
    }
    else if (s == "4")
    {
      if (status == "ON")
      {
        led4on();
      }
      else
      {
        led4off();
      }
    }
  }

  Serial.println(message);
  //server.send(200, "application/json", "");
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}
void reset_switchtime()
{
  String message = "";
  String sw;
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "sw")
    {
      sw = server.arg(i);
    }
  }
  savetime_sw(sw);
  server.send(200, "application/json", "");
}

void switchtime()
{
  String message = "";
  String sw;
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (server.argName(i) == "sw")
    {
      sw = server.arg(i);
    }
  }

  Serial.println(message);
  readfileTime_sw(sw);
  String output = switch4time(sw);
  server.send(200, "application/json", output);
}

String switch4time(String sw)
{
  //readfileTime_sw(sw);
  Serial.println("switch4time");
  //StaticJsonDocument<1500> doc;
  DynamicJsonDocument doc(1500);
  JsonArray datas = doc.createNestedArray("data");
  doc["server"] = "iot1";
  doc["func"] = "timecontrol";
  datas.add(paratojson("s1", "1", "time 1", "time", _timeschedule1, "/time?t=1&sw=" + sw + "&a=open&v=", "80"));
  datas.add(paratojson("s1", "2", "time 2", "time", _timeschedule2, "/time?t=2&sw=" + sw + "&a=close&v=", "80"));
  datas.add(paratojson("s1", "3", "time 3", "time", _timeschedule3, "/time?t=3&sw=" + sw + "&a=start&v=", "80"));
  datas.add(paratojson("s1", "4", "time 4", "time", _timeschedule4, "/time?t=4&sw=" + sw + "&a=stop&v=", "80"));
  datas.add(paratojson("s1", "5", "time 5", "time", _timeschedule5, "/time?t=5&sw=" + sw + "&a=stop&v=", "80"));
  datas.add(paratojson("s1", "6", "time 6", "time", _timeschedule6, "/time?t=6&sw=" + sw + "&a=stop&v=", "80"));
  datas.add(paratojson("s1", "7", "time 7", "time", _timeschedule7, "/time?t=7&sw=" + sw + "&a=stop&v=", "80"));
  datas.add(paratojson("s1", "8", "time 8", "time", _timeschedule8, "/time?t=8&sw=" + sw + "&a=stop&v=", "80"));

  String output = "";
  serializeJson(doc, output);
  doc.clear();
  Serial.println(output);
  return output;
}

void jsoncmd()
{
  String message = "{\"a\":\"jsoncmd\"}";
  Serial.println(message);
  //station1 = true ? "OFF" : "ON";
  String output = switch4();

  //serializeJson(doc, output);
  //doc.clear();
  Serial.println(output);
  server.send(200, "application/json", output);
}

String switch4()
{
  Serial.println("switch4");
  //StaticJsonDocument<1800> doc;
  DynamicJsonDocument doc(1800);
  JsonArray datas = doc.createNestedArray("data");
  doc["server"] = "iot1";
  doc["func"] = "switch4";
  datas.add(paratojson("s1", "1", "s1", "switch", _s1, "/onoff?s=1&status=", "80"));
  datas.add(paratojson("s1", "2", "s2", "switch", _s2, "/onoff?s=2&status=", "80"));
  datas.add(paratojson("s1", "3", "s3", "switch", _s3, "/onoff?s=3&status=", "80"));
  datas.add(paratojson("s1", "4", "s4", "switch", _s4, "/onoff?s=4&status=", "80"));
  String output = "";
  serializeJson(doc, output);
  doc.clear();
  return output;
}

StaticJsonDocument<512> paratojson(String _s, String i1, String i2, String i3, String i4, String i5, String i6)

{
  StaticJsonDocument<512> list;
  //list["v1"]["\"" + _s + "\""] = "\"" + input + "\"";
  //list["v1"]["\"f\""] = "\"" + input2 + "\"";
  list["v1"][_s] = i1;
  list["v1"]["f"] = i2;
  list["v1"]["t"] = i3;
  list["v1"]["s"] = i4;
  list["v1"]["h"] = i5;
  list["v1"]["p"] = i6;
  return list;
}

void software_Reboot()
{
  wdt_enable(WDTO_15MS); //wdt_enable(WDTO_8S);
  while (1)
  {
  }
}
