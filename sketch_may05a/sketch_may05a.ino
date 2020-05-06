/**
   Geek Factory - "Innovating together"
   Distribucion de materiales para el desarrollo e innovacion tecnologica
   www.geekfactory.mx

   Programa para leer un sensor DS18B20 sin utilizar la librería DallasTemperature.
   En este programa se puede apreciar como realizar la comunicacion con un sensor
   utilizando la librería OneWire solamente.
*/

#include <Arduino.h>
#include <OneWire.h>

/**
 * Objeto para la comunicación 1-Wire, se indica el pin a utilizar para la comunicacion
 * dentro del constructor, en este caso utilizaremos el pin 2.
 */
OneWire onewire(2);

/**
 * @brief Funcion que implementa la lectura de un sensor DS18B20.
 * 
 * @param result Apuntador a variable de tipo float para almacenar la temperatura leida del sensor.
 * @return true Retorna verdadero si la comunicación con el sensor es correcta.
 * @return false Retorna falso si no hay comunicación con el sensor.
 */
bool sensor_read(float *result)
{
  uint8_t data[12];
  int i;

  // retornar una lectura invalida si falla la comunicacion con el sensor
  *result = -100.0;

  // enviar comando de iniciar la conversion de temperatura
  // primero generamos pulso de reset
  onewire.reset();
  // enviar el comando skip ROM que selecciona todos los dispositivos en el bus
  onewire.skip();
  // enviar el comando de comienzo de conversion A/D
  onewire.write(0x44);

  // esperar el termino de conversion AD en el sensor
  delay(1000);

  // prestamos atención al pulso de presencia al generar el pulso de reset
  if (!onewire.reset())
    return false;
  // enviar el comando skip ROM que selecciona todos los dispositivos en el bus
  onewire.skip();
  // enviar comando de lectura de scratchpad
  onewire.write(0xBE);

  // comenzar lectura de datos
  for (i = 0; i < 9; i++)
    data[i] = onewire.read();

  // alinear los datos recibidos
  int16_t temp = (((int16_t)data[1]) << 11) | (((int16_t)data[0]) << 3);

  // convertir a grados centigrados
  *result = (float)temp * 0.0078125;

  // lectura satisfactoria
  return true;
}

/**
 * Funcion setup, aqui efectuamos la configuracion inicial
 */
void setup()
{
  // preparar serial
  Serial.begin(115200);
  while (!Serial)
    ;

  // mostrar mensaje
  Serial.println(F("----------------------------------------------------"));
  Serial.println(F("      DS18B20 temperature sensor test program       "));
  Serial.println(F("             https://www.geekfactory.mx             "));
  Serial.println(F("----------------------------------------------------"));
}

/**
 * Ciclo principal del programa
 */
void loop()
{
  float temperature;

  // leer la temperatura desde el DS18B20 y colocarla en la variable temperature
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
