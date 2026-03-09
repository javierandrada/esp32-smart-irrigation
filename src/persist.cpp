/**
 * @file persist.cpp
 * @brief Persistent configuration storage module
 *
 * This module uses ESP32 Preferences to store system parameters
 * in flash memory. The stored values survive system reboot.
 *
 * Stored parameters:
 *  - Minimum humidity threshold
 *  - Maximum humidity threshold
 *  - WiFi SSID
 *  - WiFi password
 */
#include "persist.h"
#include <Preferences.h>

Preferences pref;

int humedad_min = 35;
int humedad_max = 55;

String wifi_ssid = "CCT";
String wifi_pass = "gestamrosario";

/**
 * @brief Initializes persistent configuration
 *
 * Reads stored values from flash memory.
 * If values do not exist, default values are used.
 */
void persist_init() {

  pref.begin("riego");

  humedad_min = pref.getInt("hmin", 35);
  humedad_max = pref.getInt("hmax", 55);

  wifi_ssid = pref.getString("ssid", wifi_ssid);
  wifi_pass = pref.getString("pass", wifi_pass);

  pref.end();

  Serial.print("Persist -> Humedad MIN: ");
  Serial.println(humedad_min);

  Serial.print("Persist -> Humedad MAX: ");
  Serial.println(humedad_max);

  Serial.print("Persist -> SSID: ");
  Serial.println(wifi_ssid);

  Serial.print("Persist -> PASS: ");
  Serial.println(wifi_pass);
}

/**
 * @brief Stores new minimum humidity threshold
 */
int persist_get_hmin() {
  return humedad_min;
}

/**
 * @brief Stores new maximum humidity threshold
 */
int persist_get_hmax() {
  return humedad_max;
}

/**
 * @brief Stores a new minimum humidity threshold in flash memory
 */
void persist_set_hmin(int valor) {
  pref.begin("riego");
  pref.putInt("hmin", valor);
  pref.end();

  humedad_min = valor;

  Serial.print("Persist -> Nueva MIN guardada: ");
  Serial.println(valor);
}

/**
 * @brief Stores a new maximum humidity threshold in flash memory
 */
void persist_set_hmax(int valor) {
  pref.begin("riego");
  pref.putInt("hmax", valor);
  pref.end();

  humedad_max = valor;

  Serial.print("Persist -> Nueva MAX guardada: ");
  Serial.println(valor);
}

/**
 * @brief Returns the stored WiFi SSID
 */
String persist_get_ssid() {
  return wifi_ssid;
}

/**
 * @brief Returns the stored WiFi password
 */
String persist_get_pass() {
  return wifi_pass;
}

/**
 * @brief Stores a new WiFi SSID in flash memory
 */
void persist_set_ssid(String valor) {
  pref.begin("riego");
  pref.putString("ssid", valor);
  pref.end();

  wifi_ssid = valor;

  Serial.print("Persist -> Nuevo SSID guardado: ");
  Serial.println(valor);
}

/**
 * @brief Stores a new WiFi password in flash memory
 */
void persist_set_pass(String valor) {
  pref.begin("riego");
  pref.putString("pass", valor);
  pref.end();

  wifi_pass = valor;

  Serial.print("Persist -> Nuevo PASS guardado: ");
  Serial.println(valor);
}
