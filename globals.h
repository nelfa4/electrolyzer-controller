#ifndef GLOBALS_H
#define GLOBALS_H

extern bool powerOn;
extern bool powerSave;
extern float currentPWM;
extern float targetPWM;
extern float voltage;
extern float current;
extern float powerW;
extern float temperature;
extern float gasOutput;
extern bool tiltShutdown;

extern unsigned long lastSensorRead;
extern unsigned long lastDisplayUpdate;

#endif
