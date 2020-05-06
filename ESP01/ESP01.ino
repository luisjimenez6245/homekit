#include <ESP8266WiFi.h>

const char* ssid = "Mujica";
const char* password = "Mug2215562.";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();  
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  while(!client.available()){
    delay(1);
  }
  
  String req = client.readStringUntil('\r');
  req.toLowerCase();
  req.replace("http/1.1", "");
  req.replace("get", "");
  req.replace(" ", "");
  req.replace("/", "");
  String res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  client.flush();
  Serial.println(req);
  while (Serial.available())
  {
    res += Serial.readString();
  }
  client.flush();
  res += "</html>\n";

  client.print(res);
  delay(10);
}
