#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H
#include "config.h"

#ifdef TEMP_SENSOR_DS18B20
  #include <OneWire.h>
  #include <DallasTemperature.h>
#elif defined(TEMP_SENSOR_DHT22)
  #include <DHT.h>
#endif

void temp_sensor_init();
float temp_sensor_read();
#endif
