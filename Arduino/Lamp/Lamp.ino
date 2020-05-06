#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <OneWire.h>

#define DELAYVAL 500
#define PIXEL_PIN 4
#define TEMP_PIN 3
#define NUMPIXELS 16

Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
OneWire onewire(3);

uint32_t color = 16777215;
int brightness = 250;
boolean pixel_on = false;
float ant_temp = 0;
float temp = 0;
void manage_pixel()
{
  if (pixel_on)
  {
    if (brightness > 255)
    {
      brightness = 255;
    }
    if (brightness < 1)
    {
      brightness = 255;
    }
    for (int i = 0; i < NUMPIXELS; i++)
    {
      pixels.setPixelColor(i, color);
      delay(50);
      pixels.show();
    }
  }
  else
  {
    pixels.clear();
  }
  pixels.show();
}

bool sensor_read(float *result)
{
  uint8_t data[12];
  int i;
  *result = -100.0;
  onewire.reset();
  onewire.skip();
  onewire.write(0x44);
  delay(1000);
  if (!onewire.reset())
    return false;
  onewire.skip();
  onewire.write(0xBE);
  for (i = 0; i < 9; i++)
    data[i] = onewire.read();
  int16_t temp = (((int16_t)data[1]) << 11) | (((int16_t)data[0]) << 3);
  *result = (float)temp * 0.0078125;
  return true;
}

void read_temperature()
{
  float temperature;
  if (sensor_read(&temperature))
  {    
    temp = temperature;
  }
  else
  {
    temp = -100;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  pixels.begin();
  pixels.clear();
  pixels.show();
  read_temperature();
}

void manage_message(String message)
{
    
  if (message.indexOf("temperature") != -1)
  {
    Serial.println(temp);
  }
  else if (message.indexOf("statuslamp") != -1)
  {
    Serial.println(pixel_on ? "1" : "0");
  }
  else if (message.indexOf("lamp") != -1)
  {
    pixel_on = (message.indexOf("on") != -1);
    manage_pixel();
  }
  else if (message.indexOf("brightness") != -1)
  {
    message.replace("brightness", "");
    pixel_on = true;
    brightness = (atoi(message.c_str()) * 255) / 100;
    Serial.print("brightness: ");
    Serial.println(brightness);
    pixels.setBrightness(brightness);  
    pixels.show();
  }
  else if (message.indexOf("color") != -1)
  {
    message.replace("color", "");
    message.toUpperCase();
    pixel_on = true;
    Serial.print("color: ");
    color = (uint32_t)strtol(message.c_str(), NULL, 16);
    Serial.println(color, HEX);
    if (color > 16777215)
    {
      color = 16777215;
    }
    if (color < 1)
    {
      color = 1;
    }
    manage_pixel();
  }
  else
  {
  }
}

void loop()
{
  String message = "";
  while (Serial.available())
  {
    message += Serial.readString();
  }
  if (message.length() > 0)
  {
    Serial.flush();
    manage_message(message);
    Serial.flush();
  }
  read_temperature();
}
