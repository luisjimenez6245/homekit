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
boolean pixel_on = false;

void manage_pixel()
{
  if(pixel_on){
    for(int i=0; i<NUMPIXELS; i++) { 
      pixels.setPixelColor(i, pixels.Color(red_value, green_value, blue_value));
      pixels.show();   
    }
  }
  else{
    pixels.clear();
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

void setup()
{
  Serial.begin(115200);
  pixels.begin();
  pixels.clear(); 
}

void loop()
{
  float temperature;
  if (sensor_read(&temperature))
  {
    Serial.print(F("OK, Temperatura: "));
    Serial.println(temperature);
  }
  else
  {
    Serial.println(F("Fallo de comunicacion con DS18B20"));
  }
}
