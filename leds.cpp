#include "leds.h"
#include <Arduino.h>

#ifdef USE_LEDS

void leds_init() {
  ledcSetup(1, 5000, 8);
  ledcAttachPin(PIN_LED, 1);
  ledcWrite(1, 0);
}

void leds_update(unsigned long now, float currentPWM,
                 bool powerOn, bool tiltShutdown, float temperature) {
  uint8_t brightness = 0;

  if (!powerOn) {
    brightness = 0;
  } else if (tiltShutdown) {
    brightness = ((now / 167) % 2) ? 255 : 0;
  }
  #ifdef TEMP_MAX
  else if (temperature >= TEMP_MAX) {
    brightness = ((now / 167) % 2) ? 255 : 0;
  }
  #endif
  else {
    brightness = map((int)currentPWM, 0, 4095, 0, 204);
  }

  ledcWrite(1, brightness);
}

#endif
