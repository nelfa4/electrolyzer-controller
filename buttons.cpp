#include "buttons.h"
#include <Arduino.h>

bool powerOn = false;
bool powerSave = false;
float targetPWM = 0;

static bool lastPower = HIGH, lastSave = HIGH;
static unsigned long dbPower = 0, dbSave = 0;

void buttons_init() {
  pinMode(PIN_BUTTON_POWER, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SAVE, INPUT_PULLUP);
}

void buttons_read(unsigned long now) {
  bool rPower = digitalRead(PIN_BUTTON_POWER);
  bool rSave  = digitalRead(PIN_BUTTON_SAVE);

  // Кнопка ВКЛ/ВЫКЛ (она же СТОП)
  if (rPower != lastPower && now - dbPower > 50) {
    dbPower = now;
    if (rPower == LOW) {
      powerOn = !powerOn;
      if (powerOn) {
        targetPWM = powerSave ? PWM_MAX_SAVE : PWM_MAX_NORMAL;
      } else {
        targetPWM = 0;
      }
    }
  }
  lastPower = rPower;

  // Кнопка Power Save
  if (rSave != lastSave && now - dbSave > 50) {
    dbSave = now;
    if (rSave == LOW && powerOn) {
      powerSave = !powerSave;
      targetPWM = powerSave ? PWM_MAX_SAVE : PWM_MAX_NORMAL;
    }
  }
  lastSave = rSave;
}
