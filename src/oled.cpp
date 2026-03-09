/**
 * @file oled.cpp
 * @brief OLED display module
 *
 * Displays system information such as:
 * - soil humidity
 * - system mode (AUTO / MANUAL)
 * - pump status
 */
#include "oled.h"

// OLED instance
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN);

/**
 * @brief Initializes the OLED display
 */
void oled88_init(void) {
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.setFlipMode(0);
}

void oled88_clear(void) {
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void oled88_bienvenida(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_spleen6x12_mf);

  u8g2.setCursor(8, 28);
  u8g2.print("UTN Electronica II");

  u8g2.setCursor(14, 46);
  u8g2.print("Sistema de Riego");

  u8g2.sendBuffer();
}

// Keep this function for compatibility
void oled88_update(void) {
  u8g2.sendBuffer();
}

// Keep this function for compatibility
void oled88_printTit(const char * tittle) {
  u8g2.setFont(u8g2_font_spleen6x12_mf);
  u8g2.setCursor(0, 12);
  u8g2.print(tittle);
}

// Main screen for irrigation system
void oled88_printRiego(int humedad, bool modo_auto, bool bomba_on) {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_spleen6x12_mf);

  // Title
  u8g2.setCursor(0, 12);
  u8g2.print("SISTEMA DE RIEGO");

  // Humidity
  u8g2.setCursor(0, 28);
  u8g2.print("Humedad: ");
  u8g2.print(humedad);
  u8g2.print("%");

  // Mode
  u8g2.setCursor(0, 44);
  u8g2.print("Modo: ");
  if (modo_auto) {
    u8g2.print("AUTO");
  } else {
    u8g2.print("MANUAL");
  }

  // Pump state
  u8g2.setCursor(0, 60);
  u8g2.print("Bomba: ");
  if (bomba_on) {
    u8g2.print("ON");
  } else {
    u8g2.print("OFF");
  }

  u8g2.sendBuffer();
}
