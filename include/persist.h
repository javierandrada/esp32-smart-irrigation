#ifndef PERSIST_H
#define PERSIST_H

#include <Arduino.h>

void persist_init();

int persist_get_hmin();
int persist_get_hmax();

void persist_set_hmin(int valor);
void persist_set_hmax(int valor);

String persist_get_ssid();
String persist_get_pass();

void persist_set_ssid(String valor);
void persist_set_pass(String valor);

#endif
