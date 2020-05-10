#include <ESP8266WiFi.h>

const char *ssid = "Mujica";
const char *password = "Mug2215562.";

boolean led_state = false;
boolean input_state = HIGH;
void manage_state(){
  digitalWrite(LED_BUILTIN, led_state);
}

WiFiServer server(80);
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(0, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  int read_arduino_pin = digitalRead(0);
  if(read_arduino_pin == LOW){
    led_state = !led_state;
    input_state = read_arduino_pin;
    manage_state();
    delay(1000);
  }
  if (!client)
  {
    return;
  }
  String message = "";
  message = client.readStringUntil('\r');
  message.toLowerCase();
  message.replace("http/1.1", "");
  message.replace("get", "");
  message.replace(" ", "");
  message.replace("/", "");
  client.flush();
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";

  if (message.indexOf("off") != -1){
    led_state = false;
      s += "0";
  }
  else if(message.indexOf("on") != -1){
    led_state = true;
      s += "1";
  }
  else{
      s += (led_state)?"1":"0";
  }
  manage_state();
  s += "</html>\n";
  client.print(s);
  client.flush();
  delay(10);

}
