#ifndef LEDS_H
#define LEDS_H
#include "config.h"

#ifdef USE_LEDS
void leds_init();
void leds_update(unsigned long now, float currentPWM,
                 bool powerOn, bool tiltShutdown, float temperature);
#endif

#endif
