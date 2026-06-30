/*
 *  Electrolyzer Controller v4.2.1
 *  Универсальный контроллер электролизёра
 *  Настройка в config.h
 */

#include "config.h"
#include "current_sensor.h"
#include "temp_sensor.h"
#include "tilt_sensor.h"
#include "safety.h"
#include "gas_calc.h"
#include "display.h"
#include "leds.h"
#include "buttons.h"

bool powerOn = false;
bool powerSave = false;
float currentPWM = 0;
float targetPWM = 0;
float voltage = 0, current = 0, powerW = 0;
float temperature = 25.0;
float gasOutput = 0;
bool tiltShutdown = false;

unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;

void setup() {
  #if SERIAL_DEBUG
    Serial.begin(115200);
    Serial.printf("\n=== Electrolyzer v%s ===\n", FW_VERSION);
    #ifdef DISPLAY_ENABLED
      Serial.printf("Display: %dx%d\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    #else
      Serial.println("Display: OFF");
    #endif
  #endif

  // ШИМ электролизёра
  ledcSetup(0, PWM_FREQ, 12);
  ledcAttachPin(PIN_PWM_ELECTROLYSIS, 0);
  ledcWrite(0, 0);

  // Светодиоды
  #ifdef USE_LEDS
    leds_init();
  #endif

  // Кнопки
  buttons_init();

  // I2C
  #if defined(PIN_I2C_SDA) && defined(PIN_I2C_SCL)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  #endif

  // Датчики
  current_sensor_init();
  temp_sensor_init();
  safety_init();

  // Дисплей
  #ifdef DISPLAY_ENABLED
    display_init();
  #endif

  #if SERIAL_DEBUG
    Serial.println("Init complete.");
    Serial.printf("PWM Normal: %d (%.0f%%)\n", PWM_MAX_NORMAL, (float)PWM_MAX_NORMAL/40.95);
    Serial.printf("PWM Save: %d (%.0f%%)\n", PWM_MAX_SAVE, (float)PWM_MAX_SAVE/40.95);
  #endif
}

void loop() {
  unsigned long now = millis();

  // Кнопки
  buttons_read(now);

  // Датчики
  if (now - lastSensorRead >= SENSOR_INTERVAL_MS) {
    lastSensorRead = now;
    current_sensor_read(voltage, current, powerW);
    temperature = temp_sensor_read();

    if (powerOn) {
      gas_calculate(current, voltage, temperature, gasOutput);
      safety_check(now, temperature, powerOn, powerSave, tiltShutdown, targetPWM);
    }
  }

  // Плавный ШИМ
  if (powerOn && !tiltShutdown) {
    #ifdef TEMP_CONTROL_ENABLED
      if (temperature >= TEMP_MAX) {
        currentPWM += (0 - currentPWM) * 0.15;
        if (currentPWM < 2) currentPWM = 0;
      } else {
        float diff = targetPWM - currentPWM;
        currentPWM += diff * PWM_SMOOTHING;
        if (abs(diff) < 0.5) currentPWM = targetPWM;
      }
    #else
      float diff = targetPWM - currentPWM;
      currentPWM += diff * PWM_SMOOTHING;
      if (abs(diff) < 0.5) currentPWM = targetPWM;
    #endif
  } else {
    currentPWM += (0 - currentPWM) * 0.15;
    if (currentPWM < 2) currentPWM = 0;
  }
  currentPWM = constrain(currentPWM, 0, 4095);
  ledcWrite(0, (int)currentPWM);

  // Дисплей
  #ifdef DISPLAY_ENABLED
    if (now - lastDisplayUpdate >= DISPLAY_INTERVAL_MS) {
      lastDisplayUpdate = now;
      display_update(now, voltage, current, powerW, temperature,
                     gasOutput, currentPWM, powerOn, powerSave, tiltShutdown);
    }
  #endif

  // Светодиоды
  #ifdef USE_LEDS
    leds_update(now, currentPWM, powerOn, tiltShutdown, temperature);
  #endif
}
