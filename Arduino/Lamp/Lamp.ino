#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <OneWire.h>

#define DELAYVAL 500
#define PIXEL_PIN 4
#define TEMP_PIN 3
#define NUMPIXELS 16

Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
OneWire onewire(3);

int red_value = 255;
int green_value = 255;
int blue_value = 255;
int brigthness = 180;
boolean pixel_on = false;

void manage_pixel()
{
  if (pixel_on)
  {
    for (int i = 0; i < NUMPIXELS; i++)
    {
      pixels.setPixelColor(i, pixels.Color(red_value, green_value, blue_value, brigthness ));
      pixels.show();
    }
  }
  else
  {
    for (int i = 0; i < NUMPIXELS; i++)
    {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
}

bool sensor_read(float *result)
{
  uint8_t data[12];
  int i;
  *result = -100.0;
  onewire.reset();
  onewire.skip();
  onewire.write(0x44);
  delay(100);
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

void read_temperature(){
  float temperature;
  if (sensor_read(&temperature))
  {
    Serial.println(temperature);
  }
  else
  {
    Serial.println("-100");
  }
}

void setup()
{
  Serial.begin(115200);
  pixels.begin();
  pixels.clear();
  delay(5000);
}

void manage_message(String message){
  if(message.equals("temperature")){
      read_temperature();
    }
    else{
        if(message.equals("statuslamp")){
          Serial.println(pixel_on ? "1": "0");
        }
        else{
          if(message.equals("statusbrightness")){
            Serial.println((brigthness*100)/255);
          }
          else{
            if(message.equals("statuscolor")){
              Serial.println(pixels.Color(red_value, green_value, blue_value), HEX);
            }
            else{
              if(message.indexOf("lamp") != -1 ){
                pixel_on = (message.indexOf("on") != -1 );
                manage_pixel();
                Serial.println(pixel_on ? "1": "0");
              }
            }
          }
        }
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
    manage_message(message);
  }
  /*
  float temperature;
  if (sensor_read(&temperature))
  {
    Serial.print(F("OK, Temperatura: "));
    Serial.println(temperature);
  }
  else
  {
    Serial.println(F("Fallo de comunicacion con DS18B20"));
  }*/
}
