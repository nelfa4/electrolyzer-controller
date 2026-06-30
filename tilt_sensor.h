#ifndef TILT_SENSOR_H
#define TILT_SENSOR_H
#include "config.h"

#ifdef TILT_SENSOR_MPU6050
  #include <Adafruit_MPU6050.h>
  #include <Adafruit_Sensor.h>
#elif defined(TILT_SENSOR_ADXL345)
  #include <Adafruit_ADXL345_U.h>
#endif

void tilt_sensor_init();
bool tilt_sensor_read(float &angleX, float &angleY);
#endif
