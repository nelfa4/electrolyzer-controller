#include <Arduino.h>
#include "tilt_sensor.h"

#ifdef TILT_SENSOR_MPU6050
  Adafruit_MPU6050 mpu;
#elif defined(TILT_SENSOR_ADXL345)
  Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified(12345);
#endif

void tilt_sensor_init() {
  #ifdef TILT_SENSOR_MPU6050
    mpu.begin();
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);
  #elif defined(TILT_SENSOR_ADXL345)
    adxl.begin();
    adxl.setRange(ADXL345_RANGE_2_G);
  #endif
}

bool tilt_sensor_read(float &angleX, float &angleY) {
  #ifdef TILT_SENSOR_MPU6050
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);
    angleX = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
    angleY = atan2(a.acceleration.x, a.acceleration.z) * 180.0 / PI;
    return true;
  #elif defined(TILT_SENSOR_ADXL345)
    sensors_event_t event;
    adxl.getEvent(&event);
    angleX = atan2(event.acceleration.y, event.acceleration.z) * 180.0 / PI;
    angleY = atan2(event.acceleration.x, event.acceleration.z) * 180.0 / PI;
    return true;
  #else
    angleX = 0;
    angleY = 0;
    return false;
  #endif
}
