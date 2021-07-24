#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include "LittleFS.h" //https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/
//https://www.lazada.co.th/products/i2069946739-s6758312205.html
const char *ssid = "xxxx";
const char *password = "xxxxxxxx";

File configFile;

IPAddress ip(192, 168, 1, 53);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(1, 1, 1, 1); //IPAddress dns1(8, 8, 8, 8);
IPAddress dns2(1, 0, 0, 1); //IPAddress dns2(8, 8, 4, 4); (9, 9, 9,9)
ESP8266WebServer server(80);

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
String getValue(String data, char separator, int index);

void jsoncmd();
void onoff();
StaticJsonDocument<1024> paratojson(String _s, String i1, String i2, String i3, String i4, String i5, String i6);

const int relay1 = 5; //GPIO5
const int relay2 = 4; //GPIO4
const int relay3 = 0; //GPIO0
const int relay4 = 2; //GPIO2

String _s1 = "OFF";
String _s2 = "OFF";
String _s3 = "OFF";
String _s4 = "OFF";

String _timeschedule1 = "00_10_OFF";

//https://randomnerdtutorials.com/esp32-esp8266-web-server-outputs-momentary-switch/
//https://randomnerdtutorials.com/esp8266-web-server-with-arduino-ide/
// HTML web page

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
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  //WiFi.beginSmartConfig();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    //if(WiFi.smartConfigDone()){
    //Serial.println("WiFi Smart Config Done.");
    //}
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

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
  server.on("/jsoncmd", jsoncmd);
  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });
  server.onNotFound(handleNotFound);

  server.begin();

  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  File file = LittleFS.open("/switch.txt", "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
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
}
void loop() // Code under this loop runs forever.
{
  server.handleClient();
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
  server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"1; URL=/\" />");
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
  }

  if (s == "1")
  {
    if (doit == "plus")
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
    if (doit == "plus")
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
    if (doit == "plus")
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
    if (doit == "plus")
    {
      led4on();
    }
    else
    {
      led4off();
    }
  }

  Serial.println(message);
  server.send(200, "application/json", "");
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

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void jsoncmd()
{
  String message = "{\"a\":\"xxx\"}";

  Serial.println(message);

  StaticJsonDocument<1200> doc;
  JsonArray datas = doc.createNestedArray("data");
  doc["server"] = "iot1";

  datas.add(paratojson("s1", "1", "s1", "switch", _s1, "/onoff?s=1&status=", "80"));
  datas.add(paratojson("s1", "2", "s2", "switch", _s2, "/onoff?s=2&status=", "80"));
  datas.add(paratojson("s1", "3", "s3", "switch", _s3, "/onoff?s=3&status=", "80"));
  datas.add(paratojson("s1", "4", "s4", "switch", _s4, "/onoff?s=4&status=", "80"));
  datas.add(paratojson("s1", "5", "time start", "time", _timeschedule1, "/time?t=1&a=open&v=", "80"));

  //station1 = true ? "OFF" : "ON";
  String output = "";
  serializeJson(doc, output);
  doc.clear();
  Serial.println(output);
  server.send(200, "application/json", output);
}

StaticJsonDocument<1024> paratojson(String _s, String i1, String i2, String i3, String i4, String i5, String i6)
{
  StaticJsonDocument<1024> list;
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
