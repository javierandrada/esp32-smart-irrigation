#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#define SCL_PIN 22
#define SDA_PIN 21

void oled88_init(void);
void oled88_clear(void);
void oled88_bienvenida(void);
void oled88_update(void);
void oled88_printTit(const char * tittle);
void oled88_printRiego(int humedad, bool modo_auto, bool bomba_on);

#endif

