# Electrolyzer Controller

[🇷🇺 Русская версия](README_RU.md)

Universal firmware for ESP32 to control a hydrogen/oxygen electrolyzer. Features hardware PWM regulation, multi-level safety system, real-time gas output calculation, and support for various sensors and displays.

## Table of Contents

- [Features](#features)
- [Supported Hardware](#supported-hardware)
- [Quick Start](#quick-start)
- [How to Find Each Parameter](#how-to-find-each-parameter)
- [How It Works](#how-it-works)
- [Configuration](#configuration)
- [Pinout](#pinout)
- [Display](#display)
- [Gas Output Calculation](#gas-output-calculation)
- [Dependencies](#dependencies)
- [Project Structure](#project-structure)
- [Adding New Sensors](#adding-new-sensors)
- [FAQ](#faq)
- [License](#license)
- [Author](#author)

---

## Features

| Feature | Description |
|---------|-------------|
| **PWM Control** | Hardware PWM up to 15 kHz, 12-bit resolution (4096 steps) |
| **Two Modes** | Full Power and Power Save, hardware button switch |
| **Thermal Protection** | Smooth PWM reduction above 50°C, emergency cutoff at 65°C |
| **Tilt Protection** | 3-second warning before shutdown, auto-restore when leveled |
| **Gas Calculation** | Faraday's law with temperature and voltage correction, ml/min |
| **Current Monitoring** | Real-time current, voltage, power measurement |
| **TFT Display** | Status, V, I, P, gas output, temperature, PWM bar |
| **LED Indication** | Brightness proportional to power, blinks on emergency |
| **Modular Architecture** | Each sensor in its own file, easy to add or disable |
| **Flexible Configuration** | All settings in one config.h file |
| **Emergency Stop** | Power button doubles as emergency stop |

---

## Supported Hardware

### Current/Voltage Sensors (Required)

The firmware cannot work without a current sensor. Uncomment exactly one.

| #define | Sensor | Interface | Range | Accuracy | Recommended |
|---------|--------|-----------|-------|----------|-------------|
| CURRENT_SENSOR_INA226 | INA226 | I2C | 0-36V, up to 15A | ±0.1% | Yes |
| CURRENT_SENSOR_INA219 | INA219 | I2C | 0-26V, up to 3.2A | ±0.5% | For small cells |
| CURRENT_SENSOR_ACS712 | ACS712 | Analog | 5/20/30A variants | ±1.5% | Simple, no I2C |

### Temperature Sensors (Optional)

If no sensor is selected, temperature is fixed at 25°C and thermal protection is disabled.

| #define | Sensor | Interface | Range | Notes |
|---------|--------|-----------|-------|-------|
| TEMP_SENSOR_DS18B20 | DS18B20 | 1-Wire | -55 to +125°C | Waterproof, recommended |
| TEMP_SENSOR_DHT22 | DHT22 | Digital | -40 to +80°C | Slow (2s update) |
| TEMP_SENSOR_NTC | NTC 10k | Analog | -40 to +125°C | B=3950, custom circuit |

### Tilt/Position Sensors (Optional)

If no sensor is selected, tilt protection is disabled.

| #define | Sensor | Interface | Axes | Notes |
|---------|--------|-----------|------|-------|
| TILT_SENSOR_MPU6050 | MPU6050 | I2C | 6 (gyro+accel) | Most popular, recommended |
| TILT_SENSOR_ADXL345 | ADXL345 | I2C | 3 (accel only) | Simpler, cheaper |

### Displays (Optional)

If no driver is selected, display is disabled. Resolution must be set manually.

| #define | Driver | Typical Size | Typical Resolution |
|---------|--------|-------------|-------------------|
| DISPLAY_DRIVER_ILI9341 | ILI9341 | 2.4"–2.8" | 240×320 |
| DISPLAY_DRIVER_ST7789 | ST7789 | 1.8"–2.0" | 128×160, 240×240 |
| DISPLAY_DRIVER_ST7735 | ST7735 | 1.8" | 128×160 |
| DISPLAY_DRIVER_ILI9163 | ILI9163 | 1.44" | 128×128 |

Minimum recommended resolution: 128×128 (all data fits).  
Comfortable: 240×320 (data + PWM bar).

### Other Components

- ESP32 — any board with 4MB flash
- Logic-level N-channel MOSFET module — e.g., GMOS 10A 600W, XY-MOS, or any with 3.3V-compatible gate
- LED or LED array (optional) — one common pin, current up to 250mA
- Two momentary buttons — normally open, connect to GND
- INA226 module (recommended) — e.g., CJMCU-226 with 0.01Ω shunt

---

## Quick Start

### Step 1: Open config.h

All user settings are in this single file. You don't need to touch any other files.

### Step 2: Select current sensor (REQUIRED)

Uncomment one line in the CURRENT/VOLTAGE SENSOR section:

    #define CURRENT_SENSOR_INA226     // Uncomment this line
    //#define CURRENT_SENSOR_INA219   // Leave commented
    //#define CURRENT_SENSOR_ACS712   // Leave commented

If nothing is selected, compilation will fail with an error message.

### Step 3: Configure your electrolyzer

Set your parameters in the ELECTROLYZER section. See the section "How to Find Each Parameter" below for guidance.

### Step 4: Optional — enable sensors and display

Uncomment the lines for your hardware in each section. Leave everything commented if not using.

### Step 5: Set display resolution (if using display)

    #define DISPLAY_WIDTH   240
    #define DISPLAY_HEIGHT  320

### Step 6: Upload to ESP32

Open electrolyzer_controller.ino in Arduino IDE, select your ESP32 board, and upload.

---

## How to Find Each Parameter

This section explains how to measure or calculate every configurable value in config.h. No guessing — all numbers come from your actual hardware.

### PLATE_AREA_CM2 — Active area of one plate

Measure the part of the plate that is submerged in electrolyte (height × width). Do not include the part above the liquid or the terminal tab.

For a round plate: Area = π × (diameter / 2)²

Example: plate diameter 8.9 cm → 3.1416 × (8.9 / 2)² = 62.2 cm²

### CELL_COUNT — Number of cells

Count how many gaps between plates are in series. In a standard bipolar setup, this equals (number of plates − 1).

Example: 7 plates stacked with 6 gaps → CELL_COUNT = 6

### NOMINAL_CURRENT — Current at 100% PWM

This is the maximum current your power supply can deliver into your specific electrolyzer at your specific electrolyte concentration. To find it:

1. Temporarily set PWM to 100% (or connect the electrolyzer directly to the PSU)
2. Measure the current with a multimeter or the INA226
3. This is your NOMINAL_CURRENT

Why not just use the PSU rating? Because your electrolyzer has its own resistance. A 12V 20A PSU may only push 10A through your specific cell configuration.

### NOMINAL_VOLTAGE — Power supply voltage

Measure the actual voltage of your power supply under load. A "12V" battery is usually 11.5–12.6V depending on charge state.

### MAX_CURRENT — Working current in Full mode

This is your chosen safe operating current. It must be less than NOMINAL_CURRENT (otherwise PWM has no headroom).

Rule of thumb: MAX_CURRENT = NOMINAL_CURRENT × 0.8

This leaves 20% PWM headroom for the thermal controller to reduce current when needed.

Example: NOMINAL_CURRENT = 10A → MAX_CURRENT = 8A

PWM at 8A = (8 / 10) × 100% = 80% duty cycle.

### SAVE_CURRENT — Current in Power Save mode

Choose based on your desired gas output:

- 50% of MAX_CURRENT → ~50% gas output
- Example: MAX_CURRENT = 8A → SAVE_CURRENT = 5A (62.5%)

### FARADAY_EFFICIENCY — Faraday efficiency

This is the ratio of actual gas produced to theoretical maximum. To measure:

1. Run the electrolyzer at a known constant current (e.g., 5A) for a known time (e.g., 60 seconds)
2. Collect the gas in an inverted graduated cylinder or measure by water displacement
3. Calculate: Efficiency = Actual_Volume / Theoretical_Volume

Theoretical volume at 5A for 60s with 6 cells:

    H₂ = 7.6 ml/min/A × 5A × 6 cells × 1 min = 228 ml
    O₂ = 3.8 ml/min/A × 5A × 6 cells × 1 min = 114 ml
    Brown's gas = 228 + 114 = 342 ml

If you measured 246 ml: Efficiency = 246 / 342 = 0.72

Typical values: KOH 0.65–0.80, NaOH 0.60–0.75, distilled water with electrolyte 0.50–0.70

### ELECTROLYTE_CONDUCTIVITY

This is informational only — it is not used in calculations. Measure with a conductivity meter if you have one. Typical 30% KOH at 25°C: ~0.6 S/cm.

### TEMP_CRITICAL — Temperature to start reducing current

Choose based on your materials. Most plastics (PVC, acrylic) soften around 60°C. Set this 10–15°C below the softening point.

Default: 50°C

### TEMP_MAX — Emergency cutoff temperature

Set this 5–10°C below the dangerous temperature for your materials.

Default: 65°C

### TILT_ANGLE_FORWARD / BACKWARD / LEFT / RIGHT

The maximum tilt before the 3-second warning starts. Measure in degrees from vertical.

To test: tilt your electrolyzer until electrolyte risks spilling or plates become exposed. Measure the angle with your phone's inclinometer app. Set the limit 5° below that.

Default: 15°

### TILT_TIMEOUT_MS

How long the electrolyzer can be tilted before emergency shutdown. 3000ms = 3 seconds gives you time to correct the position.

### PWM_FREQ

Frequency of the PWM signal. Human hearing range is ~20 Hz to ~16 kHz. Frequencies below 16 kHz may produce audible whine from the MOSFET or wiring.

- 2000 Hz: loud whine, annoying
- 15000 Hz: barely audible to most adults
- 20000 Hz: completely silent

Higher frequencies slightly increase MOSFET switching losses. 15–20 kHz is a good balance.

Default: 15000 Hz

### PWM_SMOOTHING

How fast the PWM responds to changes. Range: 0.01 (very slow, seconds to respond) to 0.2 (very fast, almost instant).

Lower values = smoother transitions, less electrical noise.  
Higher values = faster response to commands.

Default: 0.08

---

## How It Works

### Operating Modes

| Mode | Current | Typical Gas Output (6 cells) |
|------|---------|------------------------------|
| Full | 8.0 A | ~500 ml/min |
| Save | 5.0 A | ~313 ml/min |

Modes are switched by a hardware button. PWM duty cycle is calculated automatically:

    PWM_normal = (MAX_CURRENT / NOMINAL_CURRENT) × 4095
    PWM_save   = (SAVE_CURRENT / NOMINAL_CURRENT) × 4095

### Buttons

| Button | GPIO | Function |
|--------|------|----------|
| Power | 26 | Toggle ON/OFF. OFF state stops PWM completely (emergency stop). |
| Save | 27 | Toggle between Full and Save modes. Works only when Power is ON. |

Both buttons are active LOW (connect to GND). Internal pull-up resistors are used.

### LED Indication

| State | LED Behavior |
|-------|-------------|
| Power OFF | LED off |
| Normal operation | Brightness proportional to PWM (0–80% → 0–204/255) |
| Tilt shutdown | Fast blink (3 Hz) |
| Overheat | Fast blink (3 Hz) |

### Safety System

Level 1 — Prevention: continuous monitoring of current, temperature, and tilt every 100ms.

Level 2 — Warning: smooth PWM reduction above 50°C. 3-second timer when tilted.

Level 3 — Emergency: immediate PWM cutoff above 65°C, on tilt timeout, or when Power button is pressed.

### Thermal Control Algorithm

Instead of simple ON/OFF, the firmware uses proportional control. When temperature exceeds TEMP_CRITICAL, PWM is reduced in proportion to overheating. This allows the electrolyzer to find an equilibrium temperature rather than cycling.

---

## Configuration

### Electrolyzer Parameters

| Parameter | Description | How to Find |
|-----------|-------------|-------------|
| PLATE_AREA_CM2 | Active area of one plate | Measure submerged area (cm²) |
| CELL_COUNT | Number of serial cells | Count gaps between plates |
| NOMINAL_CURRENT | Current at 100% PWM | Measure with PSU at full power |
| NOMINAL_VOLTAGE | Power supply voltage | Measure under load |
| MAX_CURRENT | Working current (Full mode) | Set ~80% of NOMINAL_CURRENT |
| SAVE_CURRENT | Working current (Save mode) | Set ~50-60% of MAX_CURRENT |
| FARADAY_EFFICIENCY | Faraday efficiency | Measure gas output / theoretical |
| ELECTROLYTE_CONDUCTIVITY | Informational only | Measure with conductivity meter |

### PWM Settings

| Parameter | Description | Default |
|-----------|-------------|---------|
| PWM_FREQ | PWM frequency in Hz | 15000 |
| PWM_RESOLUTION | PWM resolution (12-bit = 4096) | 4096 |
| PWM_SMOOTHING | Smoothing factor (0.01–0.2) | 0.08 |

### Temperature Limits

| Parameter | Description | Default |
|-----------|-------------|---------|
| TEMP_CRITICAL | Start reducing current | 50.0°C |
| TEMP_MAX | Emergency cutoff | 65.0°C |

### Tilt Angles

| Parameter | Direction | Default |
|-----------|-----------|---------|
| TILT_ANGLE_FORWARD | Forward tilt limit | 15° |
| TILT_ANGLE_BACKWARD | Backward tilt limit | 15° |
| TILT_ANGLE_LEFT | Left tilt limit | 15° |
| TILT_ANGLE_RIGHT | Right tilt limit | 15° |
| TILT_TIMEOUT_MS | Time before shutdown | 3000 ms |

---

## Pinout

| Function | ESP32 Pin | Notes |
|----------|-----------|-------|
| PWM output | GPIO 16 | Connect to MOSFET module PWM input |
| Power button | GPIO 26 | Momentary, to GND, internal pull-up |
| Save button | GPIO 27 | Momentary, to GND, internal pull-up |
| LED | GPIO 17 | Through current-limiting resistor |
| DS18B20 | GPIO 4 | 4.7k pull-up to 3.3V |
| I2C SDA | GPIO 21 | For INA226, MPU6050, etc. |
| I2C SCL | GPIO 22 | For INA226, MPU6050, etc. |
| TFT CS | GPIO 5 | SPI Chip Select |
| TFT DC | GPIO 2 | SPI Data/Command |
| TFT RST | GPIO 15 | SPI Reset |
| TFT MOSI | GPIO 23 | SPI Data |
| TFT SCK | GPIO 18 | SPI Clock |

All pins can be changed in config.h.

---

## Display

Four TFT drivers are supported. Uncomment one line in config.h.

Screen layout:

| Row | Content | Color |
|-----|---------|-------|
| 1 | Status + Mode + PWM% | White/Green/Cyan/Red |
| 2 | Voltage + Current | White |
| 3 | Power + Gas output | White |
| 4 | Temperature | White (Red if above 50°C) |
| 5 | PWM bar with percentage | Green (Red if above 70%) |

---

## Gas Output Calculation

Based on Faraday's law of electrolysis.

Theoretical H₂ at 25°C, 1 atm: ~7.6 ml/min per 1 Amp.  
O₂ = ½ H₂ (stoichiometric ratio).  
Brown's gas (H₂ + O₂) = 11.4 ml/min per 1 Amp.

Corrections applied:
- Temperature correction (linear with Kelvin)
- Voltage correction (actual vs theoretical cell voltage)
- Faraday efficiency (user-configured)
- Cell count multiplier

    Actual gas = 11.4 × Current × Cells × Efficiency × (T/298) × (V_actual / V_theoretical)

---

## Dependencies

Install via Arduino Library Manager:

| Library | Author | Required When |
|---------|--------|---------------|
| INA226 | Rob Tillaart | CURRENT_SENSOR_INA226 |
| Adafruit INA219 | Adafruit | CURRENT_SENSOR_INA219 |
| OneWire | Paul Stoffregen | TEMP_SENSOR_DS18B20 |
| DallasTemperature | Miles Burton | TEMP_SENSOR_DS18B20 |
| DHT sensor library | Adafruit | TEMP_SENSOR_DHT22 |
| Adafruit MPU6050 | Adafruit | TILT_SENSOR_MPU6050 |
| Adafruit ADXL345 | Adafruit | TILT_SENSOR_ADXL345 |
| Adafruit Unified Sensor | Adafruit | MPU6050 or ADXL345 |
| TFT_eSPI | Bodmer | Any display driver |

Important: TFT_eSPI requires configuring User_Setup.h with your pinout.

---

## Project Structure


    electrolyzer_controller/
    ├── electrolyzer_controller.ino   # Main file (setup, loop)
    ├── config.h                      # ALL user settings
    ├── buttons.h / buttons.cpp       # Power and Save buttons
    ├── leds.h / leds.cpp             # LED control (optional)
    ├── gas_calc.h / gas_calc.cpp     # Faraday gas calculation
    ├── safety.h / safety.cpp         # Thermal + tilt protection
    ├── display.h / display.cpp       # TFT display
    ├── current_sensor.h / .cpp       # INA226/INA219/ACS712
    ├── temp_sensor.h / .cpp          # DS18B20/DHT22/NTC
    └── tilt_sensor.h / .cpp          # MPU6050/ADXL345

---

## Adding New Sensors

1. Create your_sensor.h and your_sensor.cpp in the project folder
2. Add #define YOUR_SENSOR in config.h
3. Use #ifdef YOUR_SENSOR in your files
4. Include in the main .ino file
5. Submit a Pull Request to share with the community

---

## FAQ

**Q: The firmware won't compile. It says "Датчик тока не выбран!"**

A: You must uncomment one current sensor in config.h.

**Q: Can I use it without a display?**

A: Yes. Leave all display driver lines commented. Data is available via Serial (115200 baud).

**Q: Can I use it without a temperature sensor?**

A: Yes. Temperature will be fixed at 25°C, and thermal protection will be disabled.

**Q: The MOSFET makes a whining sound!**

A: Increase PWM_FREQ to 20000 or higher.

**Q: The displayed gas output seems wrong.**

A: Adjust FARADAY_EFFICIENCY to match your measurements. Typical: KOH 0.65–0.80, NaOH 0.60–0.75.

**Q: My display resolution is different.**

A: Change DISPLAY_WIDTH and DISPLAY_HEIGHT in config.h.

**Q: I want to use different pins.**

A: All pins are defined in config.h. Just change the numbers.

---

## License

This project is licensed under the MIT License — see the LICENSE file.

You are free to use, modify, distribute, and use commercially. You must include the copyright notice and license text.

---

## Author

- GitHub: @nelfa4

---

## Disclaimer

This entire project — including all source code, configuration files, documentation, and README — was generated by an artificial intelligence (neural network). The AI designed the architecture, wrote all the code, created the build system, and produced both the English and Russian documentation based on user requirements.

While the code has been tested for compilation and logical correctness, always follow proper safety protocols.
