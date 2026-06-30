#include <Arduino.h>
#include "temp_sensor.h"

#ifdef TEMP_SENSOR_DS18B20
  OneWire oneWire(PIN_ONE_WIRE);
  DallasTemperature ds18b20(&oneWire);
#elif defined(TEMP_SENSOR_DHT22)
  DHT dht(DHT22_PIN, DHT22);
#endif

void temp_sensor_init() {
  #ifdef TEMP_SENSOR_DS18B20
    ds18b20.begin();
  #elif defined(TEMP_SENSOR_DHT22)
    dht.begin();
  #endif
}

float temp_sensor_read() {
  #ifdef TEMP_SENSOR_DS18B20
    ds18b20.requestTemperatures();
    float t = ds18b20.getTempCByIndex(0);
    return (t > -127) ? t : 25.0;
  #elif defined(TEMP_SENSOR_DHT22)
    float t = dht.readTemperature();
    return isnan(t) ? 25.0 : t;
  #elif defined(TEMP_SENSOR_NTC)
    int raw = analogRead(NTC_PIN);
    float r = NTC_SERIES_R * (4095.0 / raw - 1.0);
    float steinhart = log(r / NTC_NOMINAL_R) / NTC_BETA;
    steinhart += 1.0 / 298.15;
    return (1.0 / steinhart) - 273.15;
  #else
    return 25.0;
  #endif
}
