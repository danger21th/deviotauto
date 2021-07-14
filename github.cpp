#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


const char *ssid = "xxx";
const char *password = "xxxxxxx";

// Set web server port number to 80
WiFiServer server(80);
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 3000;

void http_response(String path, WiFiClient *wclient);
JsonObject strtojson(String input);
StaticJsonDocument<1024> paratojson(String _s, String i1, String i2, String i3, String i4, String i5, String i6);
String argUrlParsing(String namearg, String LineURL);
String getValue(String data, char separator, int index);
int var1 = 10;
int var2 = 31;
int var3 = 10; //slider
String _s1 = "OFF";
String _s2 = "OFF";
String _s3 = "OFF";
String _s4 = "OFF";
String _d1 = "d1_mini_pro";
unsigned long timing = 0;
String _timeschedule1 = "00_10_OFF";
String _timeschedule2 = "04_50_OFF";
String _v1 = "00";
String _v2 = "00";
String _v3 = "OFF";
double _slider = 30;
String t = "";
String a = "";
String v = "";

void setup()
{
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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
  server.begin();
  oauth2spiffs();
}
// the loop function runs over and over again forever
void loop()
{
  //delay(1);
  WiFiClient client = server.available();
  if (client)
  {

    String LineURL = "";

    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    //while (client.connected())
    {
      currentTime = millis();
      if (client.available())
      {
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            header = " " + header;
            Serial.println("header=" + header);
            if (header.indexOf("GET /jsoncmd") > 0)
            {
              http_response("/jsoncmd", &client);
            } else if (header.indexOf("GET /slider") > 0)
            {
              t = argUrlParsing("sd", header);
              v = argUrlParsing("v", header);
              _slider = v.toDouble();
              Serial.println("_slider="+v);
              http_response("/", &client);
            }
            else if (header.indexOf("GET /time") > 0)
            {

              t = argUrlParsing("t", header);
              a = argUrlParsing("a", header);
              v = argUrlParsing("v", header);
              _v1 = v.substring(0, 2);
              _v2 = v.substring(3, 5);
              _v3 = v.substring(6, 8);
              Serial.println("v " + _v1 + " " + _v2 + " " + _v3);
              if (t == "1")
              {
                _timeschedule1 = _v1 + "_" + _v2 + "_" + _v3;
              }
              else if (t == "2")
              {
                _timeschedule2 = _v1 + "_" + _v2 + "_" + _v3;
              }
              http_response("/", &client);
              /*
Serial.println("v "+_v1+" "+_v2+" "+_v3);
               _v1 = getValue(v, '_', 0);
               _v2 = getValue(v, '_', 1);
               _v3 = getValue(v, '_', 2);

              Serial.println("v "+_v1+" "+_v2+" "+_v3);
              
             
              */
            }
            else if (header.indexOf("GET /tclose") > 0)
            {
              http_response("/tclose", &client);
              String tclose = argUrlParsing("tclose", header);
              String doit = argUrlParsing("doit", header);
              //Serial.println("cccc=" + tclose);
              if (doit == "plus")
              {
                var1 = var1 + 1;
              }
              else
              {
                var1 = var1 - 1;
              }
            }
            else if (header.indexOf("GET /topen") > 0)
            {
              http_response("/topen", &client);
              String tclose = argUrlParsing("topen", header);
              String doit = argUrlParsing("doit", header);
              //Serial.println("cccc=" + tclose);
              if (doit == "plus")
              {
                var2 = var2 + 1;
              }
              else
              {
                var2 = var2 - 1;
              }
            }
            else if (header.indexOf("GET /onoff") > 0)
            {
              String page = argUrlParsing("page", header);
              if (page == "server_status")
              {
                //http_response("/jsoncmd", &client);
                String s = argUrlParsing("s", header);
                String status = argUrlParsing("status", header);
                String doit = argUrlParsing("doit", header);
                if (s == "1")
                {
                  if (doit == "plus")
                  {
                    _s1 = "ON";
                  }
                  else
                  {
                    _s1 = "OFF";
                  }
                }
                else if (s == "2")
                {
                  if (doit == "plus")
                  {
                    _s2 = "ON";
                  }
                  else
                  {
                    _s2 = "OFF";
                  }
                }
                else if (s == "3")
                {
                  if (doit == "plus")
                  {
                    _s3 = "ON";
                  }
                  else
                  {
                    _s3 = "OFF";
                  }
                }
                else if (s == "4")
                {
                  if (doit == "plus")
                  {
                    _s4 = "ON";
                  }
                  else
                  {
                    _s4 = "OFF";
                  }
                }
                http_response("/", &client);
              }
              else if (page == "servercontrol")
              {
                http_response("/", &client);
              }
              else
              {
                http_response("/", &client);
              }
            }

            else
            {
              http_response("/", &client);
            }

            break;
            //client.println("https://randomnerdtutorials.com/esp8266-web-server/");
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }

    currentLine = "";
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void http_response(String path, WiFiClient *wclient)
{
  //Serial.println(path);
  //read2spiffs();
  if (path == "/jsoncmd")
  {
    Serial.println("jsoncmd");
    wclient->println("HTTP/1.1 200 OK");
    wclient->println("Content-Type: application/json; charset=utf-8");
    wclient->println("Server: Arduino");
    wclient->println("Connection: close");
    StaticJsonDocument<1800> doc;
    JsonArray data = doc.createNestedArray("data");
    doc["server"] = "iot1";
    data.add(paratojson("s1", "1", "time close", "button", String(var1), "/tclose?tclose=", "80"));
    data.add(paratojson("s1", "2", "time open", "button", String(var2), "/topen?topen=", "80"));
    data.add(paratojson("s1", "3", "s1", "switch", _s1, "/onoff?s=1&status=", "80"));
    data.add(paratojson("s1", "4", "s2", "switch", _s2, "/onoff?s=2&status=", "80"));
    data.add(paratojson("s1", "5", "s3", "switch", _s3, "/onoff?s=3&status=", "80"));
    data.add(paratojson("s1", "6", "s4", "switch", _s4, "/onoff?s=4&status=", "80"));
    data.add(paratojson("s1", "7", "var", "dispaly", _d1, "/var?t=", "80"));
    data.add(paratojson("s1", "8", "maxmin", "slider", String(_slider), "/slider?sd=1&v=", "80"));
    data.add(paratojson("s1", "9", "time start", "time", _timeschedule1, "/time?t=1&a=open&v=", "80"));
    data.add(paratojson("s1", "10", "time stop", "time", _timeschedule2, "/time?t=2&a=close&v=", "80"));
    //station1 = true ? "OFF" : "ON";
    String output = "";
    serializeJson(doc, output);
    doc.clear();
    //write2spiffs("xxxx");
    //read2spiffs();

    Serial.println(output);
    //wclient->println("Content-Length: "+output.length());

    wclient->println();
    wclient->println(output);
    Serial.print("getFreeHeap=");
    Serial.println(ESP.getFreeHeap(), DEC);
  }
  else if (path == "/tclose" || path == "/topen" || path == "/slider" || path == "/time")
  {
    //Serial.println("/tclose====");

    wclient->println("HTTP/1.1 200 OK");
    wclient->println("Content-Type: text/html");
    wclient->println("Connection: close"); // the connection will be closed after completion of the response

    wclient->println();
    wclient->println("<!DOCTYPE HTML>");
    wclient->println("<html>");
    wclient->println(path);
    wclient->println("</html>");
  }
  else if (path == "/")
  {
    wclient->println("HTTP/1.1 200 OK");
    wclient->println("Content-Type: text/html");
    wclient->println("Connection: close"); // the connection will be closed after completion of the response
    //wclient.println("Refresh: 5");  // refresh the page automatically every 5 sec
    wclient->println();
    wclient->println("<!DOCTYPE HTML>");
    wclient->println("<html>");
    //wclient.println("http://192.168.1.123");
    wclient->println("ok wait for write command");
    wclient->println("</html>");
  }
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

JsonObject strtojson(String input)
{
  DynamicJsonDocument doc(512);
  // You can use a String as your JSON input.
  // WARNING: the string in the input  will be duplicated in the JsonDocument.
  //String input = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  DeserializationError error = deserializeJson(doc, input);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else
  {
    Serial.println("good json");
  }
  JsonObject obj = doc.as<JsonObject>();
  return obj;
}

String argUrlParsing(String namearg, String LineURL)
{
  Serial.println(LineURL);
  if (LineURL.indexOf("?") > 0)
  {
    String argurl = LineURL.substring((LineURL.indexOf("?") + 1), (LineURL.indexOf(" HTTP") - 0));
    //Serial.println(argurl);
    StaticJsonDocument<600> argdoc;

    while (argurl.indexOf("&") > 0)
    {
      int posindex = 0;
      //Serial.println(argurl.substring(posindex,argurl.indexOf("&")));
      String tmp = argurl.substring(posindex, argurl.indexOf("&"));
      argdoc[tmp.substring(posindex, argurl.indexOf("="))] = tmp.substring((argurl.indexOf("=") + 1), tmp.length());
      posindex = argurl.indexOf("&") + 1;
      argurl = argurl.substring(posindex, argurl.indexOf(" "));
    }
    //Serial.println(argurl);
    String tmp = argurl.substring(0, argurl.length());
    argdoc[tmp.substring(0, argurl.indexOf("="))] = tmp.substring((argurl.indexOf("=") + 1), tmp.length());
    const char *sensor = argdoc[namearg];
    if (argdoc[namearg] != "")
    {
      //Serial.println(namearg+" >> "+ String(sensor));
    }
    String output;
    serializeJson(argdoc, output);
    argdoc.clear();
    Serial.println(output);
    return String(sensor);
  }
  else
  {
    return "";
  }
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