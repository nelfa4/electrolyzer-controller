#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H
#include "config.h"

#ifdef CURRENT_SENSOR_INA226
  #include <INA226.h>
  extern INA226 ina226;
#elif defined(CURRENT_SENSOR_INA219)
  #include <Adafruit_INA219.h>
  extern Adafruit_INA219 ina219;
#endif

void current_sensor_init();
void current_sensor_read(float &voltage, float &current, float &powerW);
#endif
