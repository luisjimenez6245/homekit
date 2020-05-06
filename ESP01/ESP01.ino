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
    Serial.println("");
    return;
  }
  
  while(!client.available()){
    delay(1);
  }
  
  String req = client.readStringUntil('\r').toLowerCase();
  req = req.replace("http/1.1", "").replace("get", "").replace("/", "").replace(" ","");
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  client.flush();
  Serial.print(req);
  s += Serial.readString();
  client.flush();
  s += "</html>\n";

  client.print(s);
  delay(10);
}
