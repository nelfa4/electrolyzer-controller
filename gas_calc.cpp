#include <Arduino.h>
#include "gas_calc.h"
#include "config.h"

void gas_calculate(float cur, float volt, float temp, float &gas) {
  float tempK = temp + 273.15;
  float tempFactor = tempK / 298.15;
  float baseML = cur * 11.4 * CELL_COUNT * FARADAY_EFFICIENCY * tempFactor;
  float vFactor = volt / (CELL_COUNT * 2.0);
  vFactor = constrain(vFactor, 0.8, 1.5);
  gas = baseML * vFactor;
}
