/**
 * @file wifi.cpp
 * @brief WiFi connection management module
 *
 * Handles WiFi initialization and automatic reconnection
 * using credentials stored in persistent memory.
 */

#include "wifi.h"
#include "persist.h"
#include <WiFi.h>

#define LED_WIFI 2

bool wifi_conectado = false;
unsigned long wifi_reconnect_timer = 0;
const unsigned long wifi_reconnect_interval = 5000;

/**
 * @brief Initializes WiFi connection
 *
 * Attempts to connect to the WiFi network using the
 * stored SSID and password.
 */
void wifi_init() {
  pinMode(LED_WIFI, OUTPUT);
  digitalWrite(LED_WIFI, LOW);

  String ssid = persist_get_ssid();
  String password = persist_get_pass();

  Serial.println("Iniciando WiFi...");
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long inicio = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifi_conectado = true;
    digitalWrite(LED_WIFI, HIGH);
    Serial.println();
    Serial.println();
    Serial.println("WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    wifi_conectado = false;
    digitalWrite(LED_WIFI, LOW);
    Serial.println();
    Serial.println("No se pudo conectar al WiFi");
  }
}

/**
 * @brief Forces WiFi reconnection using current stored credentials
 *
 * Disconnects from the previous network and reconnects
 * using the SSID and password stored in persist.
 */
void wifi_reconnect_now() {
  String ssid = persist_get_ssid();
  String password = persist_get_pass();

  Serial.println("Aplicando nueva configuracion WiFi...");
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  wifi_conectado = false;
  digitalWrite(LED_WIFI, LOW);

  WiFi.disconnect(true, true);
  delay(500);

  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long inicio = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifi_conectado = true;
    digitalWrite(LED_WIFI, HIGH);
    Serial.println();
    Serial.println("WiFi reconectado con la nueva red");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    wifi_conectado = false;
    digitalWrite(LED_WIFI, LOW);
    Serial.println();
    Serial.println("No se pudo conectar a la nueva red");
  }
}

/**
 * @brief Maintains WiFi connection
 *
 * Periodically checks the connection status and
 * attempts reconnection if WiFi is lost.
 */
void wifi_loop() {
  if (WiFi.status() == WL_CONNECTED) {
    wifi_conectado = true;
    digitalWrite(LED_WIFI, HIGH);
    return;
  }

  wifi_conectado = false;
  digitalWrite(LED_WIFI, LOW);

  unsigned long ahora = millis();

  // Attempt WiFi reconnection every 5 seconds
  if (ahora - wifi_reconnect_timer >= wifi_reconnect_interval) {
    wifi_reconnect_timer = ahora;

    String ssid = persist_get_ssid();
    String password = persist_get_pass();

    Serial.println("WiFi desconectado, reconectando...");
    Serial.print("Conectando a: ");
    Serial.println(ssid);

    WiFi.disconnect();
    WiFi.begin(ssid.c_str(), password.c_str());
  }
}
