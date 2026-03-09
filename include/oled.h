#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "config_oled.h"

#define SCL_PIN I2C_SCL_PIN
#define SDA_PIN I2C_SDA_PIN

void oled88_init(void);
void oled88_clear(void);
void oled88_bienvenida(void);
void oled88_update(void);
void oled88_printTit(const char * tittle);
void oled88_printRiego(int humedad, bool modo_auto, bool bomba_on);

#endif
