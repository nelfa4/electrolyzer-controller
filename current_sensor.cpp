#include <Arduino.h>
#include "current_sensor.h"

#ifdef CURRENT_SENSOR_INA226
  INA226 ina226(0x40);
#elif defined(CURRENT_SENSOR_INA219)
  Adafruit_INA219 ina219;
#endif

void current_sensor_init() {
  #ifdef CURRENT_SENSOR_INA226
    ina226.begin();
    ina226.setMaxCurrentShunt(15.0, 0.01);
  #elif defined(CURRENT_SENSOR_INA219)
    ina219.begin();
  #endif
}

void current_sensor_read(float &voltage, float &current, float &powerW) {
  #ifdef CURRENT_SENSOR_INA226
    voltage = ina226.getBusVoltage();
    current = abs(ina226.getCurrent());
  #elif defined(CURRENT_SENSOR_INA219)
    voltage = ina219.getBusVoltage_V();
    current = ina219.getCurrent_mA() / 1000.0;
  #elif defined(CURRENT_SENSOR_ACS712)
    int raw = analogRead(ACS712_PIN);
    float mv = raw * (3300.0 / 4095.0);
    current = (mv - 1650.0) / ACS712_MV_PER_AMP;
    voltage = 12.0;
  #else
    voltage = 12.0;
    current = 0;
  #endif

  if (current < 0.02) current = 0;
  powerW = voltage * current;
}
