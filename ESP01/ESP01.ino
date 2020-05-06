#include <ESP8266WiFi.h>

const char *ssid = "Mujica";
const char *password = "Mug2215562.";

WiFiServer server(80);
uint32_t color = 16777215;
int brightness = 180;
boolean pixel_on = false;
float temperature;
String message = "";
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.setTimeout(2000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  message = "temperature";
  manage_arduino();
  delay(5000);
  server.begin();
}

void manage_wifi_client(WiFiClient client)
{
  String res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  if (message.indexOf("temperature") != -1)
  {
    res += temperature;
  }
  else if (message.indexOf("statuslamp") != -1)
  {
    res += (pixel_on ? "1" : "0");
  }
  else if (message.indexOf("statusbrightness") != -1)
  {
    res += brightness;
  }
  else if (message.indexOf("statuscolor") != -1)
  {
    res += String(color, HEX);
  }
  else if (message.indexOf("lamp") != -1)
  {
    pixel_on = (message.indexOf("on") != -1);
    res += (pixel_on ? "1" : "0");
  }
  else if (message.indexOf("brightness") != -1)
  {
    message.replace("brightness", "");
    pixel_on = true;
    brightness = atoi(message.c_str());
    res += brightness;
  }
  else if (message.indexOf("color") != -1)
  {
    message.replace("color", "");
    message.toUpperCase();
    pixel_on = true;
    color = (uint32_t)strtol(message.c_str(), NULL, 16);
    res += String(color, HEX);
  }
  else
  {
    Serial.println(message);
  }
  client.flush();
  res += "</html>\n";
  client.print(res);
}

void manage_arduino()
{
  if (message.indexOf("temperature") != -1 || message.indexOf("lamp") != -1)
  {
    String res = "";
    while (Serial.available())
    {
      res += Serial.readString();
    }
    Serial.flush();
    if (message.indexOf("temperature") != -1)
    {
      temperature = atoi(res.c_str());
    }
    else if (message.indexOf("lamp") != -1)
    {
      pixel_on = (message.indexOf("on") != -1);
      res += (pixel_on ? "1" : "0");
    }
  }
  Serial.flush();
}

void loop()
{
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  while (!client.available())
  {
    delay(1);
  }
  message = "";
  message = client.readStringUntil('\r');
  message.toLowerCase();
  message.replace("http/1.1", "");
  message.replace("get", "");
  message.replace(" ", "");
  message.replace("/", "");
  Serial.println(message);
  Serial.flush();
  manage_wifi_client(client);
  client.flush();
  manage_arduino();
}
