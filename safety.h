#ifndef SAFETY_H
#define SAFETY_H
#include "config.h"

void safety_init();
void safety_check(unsigned long now, float temperature,
                  bool powerOn, bool powerSave,
                  bool &tiltShutdown, float &targetPWM);
#endif
