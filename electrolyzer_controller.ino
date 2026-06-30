#include "config.h"
#include "globals.h"
#include "current_sensor.h"
#include "temp_sensor.h"
#include "tilt_sensor.h"
#include "safety.h"
#include "gas_calc.h"
#include "display.h"
#include "leds.h"
#include "buttons.h"

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

  ledcSetup(0, PWM_FREQ, 12);
  ledcAttachPin(PIN_PWM_ELECTROLYSIS, 0);
  ledcWrite(0, 0);

  #ifdef USE_LEDS
    leds_init();
  #endif

  buttons_init();

  #if defined(PIN_I2C_SDA) && defined(PIN_I2C_SCL)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  #endif

  current_sensor_init();
  temp_sensor_init();
  safety_init();

  #ifdef DISPLAY_ENABLED
    display_init();
  #endif

  #if SERIAL_DEBUG
    Serial.println("Init complete.");
  #endif
}

void loop() {
  unsigned long now = millis();

  buttons_read(now);

  if (now - lastSensorRead >= SENSOR_INTERVAL_MS) {
    lastSensorRead = now;
    current_sensor_read(voltage, current, powerW);
    temperature = temp_sensor_read();

    if (powerOn) {
      gas_calculate(current, voltage, temperature, gasOutput);
      safety_check(now);
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

  #ifdef DISPLAY_ENABLED
    if (now - lastDisplayUpdate >= DISPLAY_INTERVAL_MS) {
      lastDisplayUpdate = now;
      display_update(now);
    }
  #endif

  #ifdef USE_LEDS
    leds_update(now);
  #endif
}
