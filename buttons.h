#ifndef BUTTONS_H
#define BUTTONS_H
#include "config.h"

void buttons_init();
void buttons_read(unsigned long now);

extern bool powerOn;
extern bool powerSave;
extern float targetPWM;
#endif
