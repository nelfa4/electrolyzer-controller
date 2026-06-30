#include <Arduino.h>
#include "globals.h"

bool powerOn = false;
bool powerSave = false;
float currentPWM = 0;
float targetPWM = 0;
float voltage = 0;
float current = 0;
float powerW = 0;
float temperature = 25.0;
float gasOutput = 0;
bool tiltShutdown = false;

unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
