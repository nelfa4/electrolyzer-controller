#include <Arduino.h>
#include "display.h"
#include "globals.h"

#ifdef DISPLAY_ENABLED

TFT_eSPI disp = TFT_eSPI();

#define COLOR_BG      TFT_BLACK
#define COLOR_TEXT    TFT_WHITE
#define COLOR_OK      TFT_GREEN
#define COLOR_ERR     TFT_RED
#define COLOR_INFO    TFT_CYAN
#define COLOR_BAR     TFT_DARKGREY

void display_init() {
  disp.init();
  disp.setRotation(DISPLAY_ROTATION);
  disp.fillScreen(COLOR_BG);
  disp.setTextColor(COLOR_TEXT, COLOR_BG);
  disp.setTextSize(2);
  disp.setCursor(5, 5);
  disp.println("Electrolyzer");
  disp.println("v" FW_VERSION);
}

void display_update(unsigned long now) {
  disp.fillScreen(COLOR_BG);
  disp.setTextSize(2);

  int y = 2;
  int rowH = disp.fontHeight() + 2;

  disp.setCursor(0, y);
  disp.setTextColor(COLOR_TEXT, COLOR_BG);
  if (!powerOn) {
    disp.print("STATUS: OFF");
  } else if (tiltShutdown) {
    disp.setTextColor(COLOR_ERR, COLOR_BG);
    disp.print("! TILT STOP !");
  }
  #ifdef TEMP_CONTROL_ENABLED
  else if (temperature >= TEMP_MAX) {
    disp.setTextColor(COLOR_ERR, COLOR_BG);
    disp.print("! OVERHEAT !");
  }
  #endif
  else {
    disp.setTextColor(powerSave ? COLOR_OK : COLOR_INFO, COLOR_BG);
    disp.print(powerSave ? "SAVE " : "FULL ");
    disp.print((int)(currentPWM * 100 / 4095));
    disp.print("%");
  }
  y += rowH;

  disp.setTextColor(COLOR_TEXT, COLOR_BG);
  disp.setCursor(0, y);
  disp.print("V:"); disp.print(voltage, 1); disp.print("V");
  disp.print(" I:"); disp.print(current, 2); disp.print("A");
  y += rowH;

  disp.setCursor(0, y);
  disp.print("P:"); disp.print(powerW, 1); disp.print("W");
  disp.print(" G:"); disp.print(gasOutput, 0); disp.print("ml");
  y += rowH;

  disp.setCursor(0, y);
  #ifdef TEMP_CONTROL_ENABLED
    if (temperature > TEMP_CRITICAL)
      disp.setTextColor(COLOR_ERR, COLOR_BG);
    else
      disp.setTextColor(COLOR_TEXT, COLOR_BG);
  #else
    disp.setTextColor(COLOR_TEXT, COLOR_BG);
  #endif
  disp.print("T:"); disp.print(temperature, 1); disp.print("C");
  y += rowH;

  disp.setTextColor(COLOR_TEXT, COLOR_BG);
  disp.setCursor(0, y);
  disp.print("PWM:");
  int pct = (int)(currentPWM * 100 / 4095);
  int barMax = DISPLAY_WIDTH - 90;
  if (barMax < 10) barMax = 10;
  int barW = map(pct, 0, 100, 0, barMax);
  disp.drawRect(55, y + 3, barMax + 4, 14, COLOR_BAR);
  disp.fillRect(57, y + 5, barW, 10, pct > 70 ? COLOR_ERR : COLOR_OK);
  disp.setCursor(DISPLAY_WIDTH - 40, y);
  disp.print(pct); disp.print("%");
}

#endif
