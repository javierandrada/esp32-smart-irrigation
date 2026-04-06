/**
 * Smart Irrigation System - ESP32
 *
 * Features:
 *  - Soil humidity monitoring
 *  - Automatic irrigation control
 *  - Manual control via push button
 *  - Remote control via MQTT
 *  - OLED status display
 *  - Persistent configuration storage
 *  - WiFi connectivity
 *
 * Author: Javier Andrada
 * UTN FRRO – Embedded Systems
 */
#include "wifi.h"
#include "sensor_humedad.h"
#include "bomba.h"
#include "pulsadores.h"
#include "oled.h"
#include "mqtt.h"
#include "persist.h"

// Finite State Machine for system mode
enum ModoSistema {
  MODO_AUTOMATICO,
  MODO_MANUAL
};

// Current FSM state
ModoSistema modo_actual = MODO_AUTOMATICO;

// Used to detect mode changes
bool modo_anterior_auto = true;

// Timing variables
unsigned long tiempo_reporte_serial = 0;
unsigned long tiempo_oled = 0;

const unsigned long intervalo_serial = 1000;
const unsigned long intervalo_oled = 500;

// Last humidity value
int humedad = 0;

// Valid humidity flag
bool humedad_valida = false;

// Pump state tracking for MQTT
bool estado_bomba_anterior = false;
bool estado_bomba_inicial_publicado = false;

/**
 * @brief System initialization
 *
 * Initializes all modules:
 * - persistent configuration
 * - WiFi connection
 * - humidity sensor
 * - pump control
 * - push buttons
 * - OLED display
 * - MQTT communication
 */
void setup() {

  Serial.begin(115200);

  Serial.println("STARTING PROGRAM");

  persist_init();
  wifi_init();
  sensor_humedad_init();
  bomba_init();
  pulsadores_init();
  oled88_init();
  oled88_bienvenida();
  delay(2000);

  mqtt_init();

  modo_actual = MODO_AUTOMATICO;
}

/**
 * @brief Main execution loop
 *
 * Executes non-blocking tasks and manages system logic:
 * - WiFi maintenance
 * - MQTT communication
 * - sensor reading
 * - FSM irrigation control
 * - OLED updates
 * - serial monitoring
 */
void loop() {

  // Run all non-blocking tasks
  wifi_loop();
  pulsadores_loop();
  sensor_humedad_loop();
  mqtt_loop();

  // Publish initial pump state only once
  if (!estado_bomba_inicial_publicado) {
    mqtt_publicar_bomba(bomba_estado());
    estado_bomba_anterior = bomba_estado();
    estado_bomba_inicial_publicado = true;
  }

  // Read mode from button module
  bool modo_auto = modo_automatico();

  // Detect mode change
  if (modo_auto != modo_anterior_auto) {

    if (modo_auto) {
      modo_actual = MODO_AUTOMATICO;
      Serial.println("FSM -> AUTOMATIC MODE");
    } else {
      modo_actual = MODO_MANUAL;
      Serial.println("FSM -> MANUAL MODE");
    }

    modo_anterior_auto = modo_auto;
  }

  // Update humidity only when a full averaged value is ready
  if (sensor_humedad_nuevo_dato()) {
    humedad = sensor_humedad_porcentaje();
    humedad_valida = true;
    mqtt_publicar_humedad(humedad);
  }

  // FSM main logic
  switch (modo_actual) {

    case MODO_AUTOMATICO:
      // Automatic irrigation control using humidity hysteresis
      // Pump turns ON when humidity < minimum threshold
      // Pump turns OFF when humidity > maximum threshold
      if (humedad_valida) {
        if (humedad < persist_get_hmin() && !bomba_estado()) {
          bomba_encender();
        }

        if (humedad > persist_get_hmax() && bomba_estado()) {
          bomba_apagar();
        }
      }
      break;

    case MODO_MANUAL:
      if (mqtt_hay_comando()) {

        if (mqtt_comando_encender()) {
          bomba_encender();
          Serial.println("MQTT -> Comando ON ejecutado");
        }

        if (mqtt_comando_apagar()) {
          bomba_apagar();
          Serial.println("MQTT -> Comando OFF ejecutado");
        }

        mqtt_limpiar_comando();
      }
      break;
  }

  // Publish pump state only when it changes
  bool estado_bomba_actual = bomba_estado();

  if (estado_bomba_actual != estado_bomba_anterior) {
    mqtt_publicar_bomba(estado_bomba_actual);
    estado_bomba_anterior = estado_bomba_actual;
  }

  unsigned long ahora = millis();

  // OLED update
  if (ahora - tiempo_oled >= intervalo_oled) {
    tiempo_oled = ahora;
    oled88_printRiego(humedad, modo_auto, bomba_estado());
  }

  // Serial report
  if (ahora - tiempo_reporte_serial >= intervalo_serial) {
    tiempo_reporte_serial = ahora;

    Serial.print("Soil humidity: ");
    Serial.print(humedad);
    Serial.println("%");

    Serial.print("Mode: ");
    if (modo_actual == MODO_AUTOMATICO)
      Serial.println("AUTO");
    else
      Serial.println("MANUAL");

    Serial.print("Pump: ");
    if (bomba_estado())
      Serial.println("ON");
    else
      Serial.println("OFF");

    Serial.println("----------------------");
  }

  // Serial commands
  if (Serial.available()) {

    String comando = Serial.readStringUntil('\n');
    comando.trim();

    Serial.print("Comando recibido: ");
    Serial.println(comando);

    if (comando.startsWith("hmin ")) {
      int valor = comando.substring(5).toInt();
      persist_set_hmin(valor);
    }

    else if (comando.startsWith("hmax ")) {
      int valor = comando.substring(5).toInt();
      persist_set_hmax(valor);
    }

    else if (comando.startsWith("ssid ")) {
      String valor = comando.substring(5);
      valor.trim();
      persist_set_ssid(valor);
      Serial.println("SSID guardado. Escriba 'reconectar' para aplicar cambios.");
    }

    else if (comando.startsWith("pass ")) {
      String valor = comando.substring(5);
      valor.trim();
      persist_set_pass(valor);
      Serial.println("PASS guardado. Escriba 'reconectar' para aplicar cambios.");
    }

    else if (comando == "reconectar") {
      wifi_reconnect_now();
    }

    else {
      Serial.println("Comando no reconocido");
      Serial.println("Use:");
      Serial.println("hmin <valor>");
      Serial.println("hmax <valor>");
      Serial.println("ssid <nombre_red>");
      Serial.println("pass <clave>");
      Serial.println("reconectar");
    }
  }
}
