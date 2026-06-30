#include "gas_calc.h"
#include <Arduino.h>

void gas_calculate(float current, float voltage, float temperature, float &gasOutput) {
  float tempK = temperature + 273.15;
  float tempFactor = tempK / 298.15;
  float baseML = current * 11.4 * CELL_COUNT * FARADAY_EFFICIENCY * tempFactor;
  float vFactor = (voltage / (CELL_COUNT * 2.0));
  vFactor = constrain(vFactor, 0.8, 1.5);
  gasOutput = baseML * vFactor;
}
