#ifndef DISPLAY_H
#define DISPLAY_H
#include "config.h"

// Определяем, включён ли хоть один TFT-драйвер
#if defined(DISPLAY_DRIVER_ILI9341) || defined(DISPLAY_DRIVER_ST7789) || \
    defined(DISPLAY_DRIVER_ST7735)  || defined(DISPLAY_DRIVER_ILI9163)
  #define DISPLAY_ENABLED 1
  #include <TFT_eSPI.h>
#endif

#ifdef DISPLAY_ENABLED
void display_init();
void display_update(unsigned long now,
                    float voltage, float current, float powerW,
                    float temperature, float gasOutput,
                    float currentPWM, bool powerOn, bool powerSave,
                    bool tiltShutdown);
#endif

#endif
