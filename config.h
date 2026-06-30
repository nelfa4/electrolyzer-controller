#ifndef CONFIG_H
#define CONFIG_H

// ==================== ВЕРСИЯ ====================
#define FW_VERSION "4.2.2"

// ==================== ДИСПЛЕЙ (TFT) ====================
// Раскомментируйте ОДНУ строку с вашим драйвером:
//#define DISPLAY_DRIVER_ILI9341   
//#define DISPLAY_DRIVER_ST7789    
//#define DISPLAY_DRIVER_ST7735    
//#define DISPLAY_DRIVER_ILI9163
// Если ничего не раскомментировано — дисплей отключен

// Разрешение дисплея (введите ВРУЧНУЮ):
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  320
// Минимальное рекомендуемое: 128x128
// Комфортное: 240x320

// Поворот дисплея (0-3):
#define DISPLAY_ROTATION 1

// ==================== ДАТЧИК ТОКА/НАПРЯЖЕНИЯ (ОБЯЗАТЕЛЕН) ====================
// Раскомментируйте ОДНУ строку:
//#define CURRENT_SENSOR_INA226     // I2C, до 36В,
//#define CURRENT_SENSOR_INA219   // I2C, до 26В, 3.2А max с onboard шунтом
//#define CURRENT_SENSOR_ACS712   // Аналоговый, 5A/20A/30A версии

// Если ничего не выбрано — ошибка компиляции:
#if !defined(CURRENT_SENSOR_INA226) && !defined(CURRENT_SENSOR_INA219) && !defined(CURRENT_SENSOR_ACS712)
  #error "Датчик тока не выбран! Раскомментируйте одну строку в секции ДАТЧИК ТОКА"
#endif

// Версия ACS712 (только если выбран ACS712):
#ifdef CURRENT_SENSOR_ACS712
  #define ACS712_MV_PER_AMP 185   // 185 для 5A, 100 для 20A, 66 для 30A
  #define ACS712_PIN         34
#endif

// ==================== ДАТЧИК ТЕМПЕРАТУРЫ ====================
// Раскомментируйте ОДНУ строку (или оставьте все закомментированными):
//#define TEMP_SENSOR_DS18B20     // 1-Wire, герметичный, до 125°C
//#define TEMP_SENSOR_DHT22       // Цифровой, до 80°C
//#define TEMP_SENSOR_NTC         // Термистор 10кОм, аналоговый
// Если ничего не выбрано — температура всегда 25°C, защита отключена

// Пины и параметры (только если выбран соответствующий датчик):
#ifdef TEMP_SENSOR_DS18B20
  #define PIN_ONE_WIRE 4
#endif

#ifdef TEMP_SENSOR_DHT22
  #define DHT22_PIN 4
#endif

#ifdef TEMP_SENSOR_NTC
  #define NTC_PIN         35
  #define NTC_SERIES_R    10000
  #define NTC_NOMINAL_R   10000
  #define NTC_BETA        3950
#endif

// ==================== ДАТЧИК НАКЛОНА (ГИРОСКОП) ====================
// Раскомментируйте ОДНУ строку (или оставьте все закомментированными):
//#define TILT_SENSOR_MPU6050     // 6-осевой гироскоп+акселерометр, I2C
//#define TILT_SENSOR_ADXL345     // 3-осевой акселерометр, I2C
// Если ничего не выбрано — защита по наклону ОТКЛЮЧЕНА

// Углы наклона (только если датчик наклона выбран):
#if defined(TILT_SENSOR_MPU6050) || defined(TILT_SENSOR_ADXL345)
  #define TILT_ANGLE_FORWARD     15
  #define TILT_ANGLE_BACKWARD    15
  #define TILT_ANGLE_LEFT        15
  #define TILT_ANGLE_RIGHT       15
  #define TILT_TIMEOUT_MS        3000
#endif

// ==================== СВЕТОДИОДЫ ====================
// Раскомментируйте, если нужна светодиодная индикация:
#define USE_LEDS

#ifdef USE_LEDS
  #define PIN_LED 17
#endif

// ==================== ПИНЫ ====================
#define PIN_PWM_ELECTROLYSIS   16
#define PIN_BUTTON_POWER       26     // ВКЛ/ВЫКЛ 
#define PIN_BUTTON_SAVE        27     // Переключение режима Power Save

// TFT пины (только если выбран дисплей):
#if defined(DISPLAY_DRIVER_ILI9341) || defined(DISPLAY_DRIVER_ST7789) || \
    defined(DISPLAY_DRIVER_ST7735)  || defined(DISPLAY_DRIVER_ILI9163)
  #define PIN_TFT_CS     5
  #define PIN_TFT_DC     2
  #define PIN_TFT_RST    15
  #define PIN_TFT_MOSI   23
  #define PIN_TFT_SCLK   18
#endif

// I2C пины (только если есть I2C датчики):
#if defined(CURRENT_SENSOR_INA226) || defined(CURRENT_SENSOR_INA219) || \
    defined(TILT_SENSOR_MPU6050)  || defined(TILT_SENSOR_ADXL345)
  #define PIN_I2C_SDA    21
  #define PIN_I2C_SCL    22
#endif

// ==================== ЭЛЕКТРОЛИЗЁР ====================
#define PLATE_AREA_CM2          62.0
#define CELL_COUNT               6
#define NOMINAL_CURRENT         10.0    // Ток при 100% ШИМ
#define NOMINAL_VOLTAGE         12.0    // Напряжение питания
#define MAX_CURRENT              8.0    // Рабочий ток
#define SAVE_CURRENT             5.0    // Ток в Power Save
#define FARADAY_EFFICIENCY       0.72
#define ELECTROLYTE_CONDUCTIVITY 0.4

// ==================== PWM ====================
#define PWM_FREQ               15000
#define PWM_RESOLUTION         4096
#define PWM_MAX_NORMAL         ((uint16_t)((MAX_CURRENT / NOMINAL_CURRENT) * 4095.0))
#define PWM_MAX_SAVE           ((uint16_t)((SAVE_CURRENT / NOMINAL_CURRENT) * 4095.0))

// ==================== ТЕМПЕРАТУРНАЯ ЗАЩИТА ====================
#if defined(TEMP_SENSOR_DS18B20) || defined(TEMP_SENSOR_DHT22) || defined(TEMP_SENSOR_NTC)
  #define TEMP_CRITICAL          50.0
  #define TEMP_MAX               65.0
  #define TEMP_CONTROL_ENABLED   1
#endif

// ==================== СЛУЖЕБНЫЕ ====================
#define SERIAL_DEBUG            1
#define SENSOR_INTERVAL_MS     100
#define DISPLAY_INTERVAL_MS    250
#define PWM_SMOOTHING          0.08

#endif
