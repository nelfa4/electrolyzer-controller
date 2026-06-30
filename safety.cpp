#include "safety.h"
#include "tilt_sensor.h"
#include <Arduino.h>

#if defined(TILT_SENSOR_MPU6050) || defined(TILT_SENSOR_ADXL345)
static bool tiltActive = false;
static unsigned long tiltStart = 0;
#endif

void safety_init() {
  tilt_sensor_init();
}

void safety_check(unsigned long now, float temperature,
                  bool powerOn, bool powerSave,
                  bool &tiltShutdown, float &targetPWM) {
  if (!powerOn) return;

  // Термоконтроль
  #ifdef TEMP_CONTROL_ENABLED
    if (temperature >= TEMP_MAX) {
      targetPWM = 0;
      return;
    }

    if (temperature > TEMP_CRITICAL) {
      float overheat = temperature - TEMP_CRITICAL;
      float range = TEMP_MAX - TEMP_CRITICAL;
      float factor = 1.0 - (overheat / range);
      factor = constrain(factor, 0.05, 1.0);
      float maxPWM = powerSave ? PWM_MAX_SAVE : PWM_MAX_NORMAL;
      targetPWM = maxPWM * factor;
      if (targetPWM < 20) targetPWM = 20;
    } else if (temperature < TEMP_CRITICAL - 2.0) {
      targetPWM = powerSave ? PWM_MAX_SAVE : PWM_MAX_NORMAL;
    }
  #endif

  // Контроль наклона
  #if defined(TILT_SENSOR_MPU6050) || defined(TILT_SENSOR_ADXL345)
    float ax = 0, ay = 0;
    tilt_sensor_read(ax, ay);

    bool tilted = (ax > TILT_ANGLE_FORWARD) || (ax < -TILT_ANGLE_BACKWARD) ||
                  (ay > TILT_ANGLE_RIGHT)   || (ay < -TILT_ANGLE_LEFT);

    if (tilted && !tiltShutdown) {
      if (!tiltActive) {
        tiltStart = now;
        tiltActive = true;
      }
      if (now - tiltStart >= TILT_TIMEOUT_MS) {
        tiltShutdown = true;
        targetPWM = 0;
      }
    } else if (!tilted) {
      tiltActive = false;
      if (tiltShutdown) {
        tiltShutdown = false;
        #ifdef TEMP_CONTROL_ENABLED
          if (temperature < TEMP_CRITICAL)
        #endif
            targetPWM = powerSave ? PWM_MAX_SAVE : PWM_MAX_NORMAL;
      }
    }
  #endif
}
