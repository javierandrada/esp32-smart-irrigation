/**
 * Smart Irrigation System - ESP32
 */

#include "wifi.h"
#include "sensor_humedad.h"
#include "bomba.h"
#include "pulsadores.h"
#include "oled.h"
#include "mqtt.h"
#include "persist.h"

// FSM
enum ModoSistema {
  MODO_AUTOMATICO,
  MODO_MANUAL
};

ModoSistema modo_actual = MODO_AUTOMATICO;
bool modo_anterior_auto = true;

// Timing
unsigned long tiempo_reporte_serial = 0;
unsigned long tiempo_oled = 0;

const unsigned long intervalo_serial = 1000;
const unsigned long intervalo_oled = 500;

// Variables
int humedad = 0;
bool humedad_valida = false;

bool estado_bomba_anterior = false;
bool estado_bomba_inicial_publicado = false;


// 🔧 ================= FUNCION SERIAL =================
void procesar_comando_serial() {
  static char buffer[64];
  
  if (Serial.available()) {
    int len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    if (len == 0) return;

    buffer[len] = '\0';

    Serial.print("Comando recibido: ");
    Serial.println(buffer);

    // hmin
    if (strncmp(buffer, "hmin ", 5) == 0) {
      int valor = atoi(&buffer[5]);

      if (valor >= 0 && valor <= 100) {
        persist_set_hmin(valor);
      } else {
        Serial.println("Error: hmin fuera de rango (0-100)");
      }
    }

    // hmax
    else if (strncmp(buffer, "hmax ", 5) == 0) {
      int valor = atoi(&buffer[5]);

      if (valor >= 0 && valor <= 100) {
        persist_set_hmax(valor);
      } else {
        Serial.println("Error: hmax fuera de rango (0-100)");
      }
    }

    // ssid
    else if (strncmp(buffer, "ssid ", 5) == 0) {
      char *valor = &buffer[5];
      persist_set_ssid(valor);
      Serial.println("SSID guardado. Escriba 'reconectar' para aplicar cambios.");
    }

    // pass
    else if (strncmp(buffer, "pass ", 5) == 0) {
      char *valor = &buffer[5];
      persist_set_pass(valor);
      Serial.println("PASS guardado. Escriba 'reconectar' para aplicar cambios.");
    }

    // reconectar
    else if (strcmp(buffer, "reconectar") == 0) {
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
// 🔧 ==================================================


// SETUP
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


// LOOP
void loop() {

  wifi_loop();
  pulsadores_loop();
  sensor_humedad_loop();
  mqtt_loop();

  // Publicar estado inicial
  if (!estado_bomba_inicial_publicado) {
    mqtt_publicar_bomba(bomba_estado());
    estado_bomba_anterior = bomba_estado();
    estado_bomba_inicial_publicado = true;
  }

  // Modo
  bool modo_auto = modo_automatico();

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

  // Lectura sensor
  if (sensor_humedad_nuevo_dato()) {
    humedad = sensor_humedad_porcentaje();
    humedad_valida = true;
    mqtt_publicar_humedad(humedad);
  }

  // Control FSM
  switch (modo_actual) {

    case MODO_AUTOMATICO:
      if (humedad_valida) {
        int hmin = persist_get_hmin();
        int hmax = persist_get_hmax();

        if (humedad < hmin && !bomba_estado()) {
          bomba_encender();
        }

        if (humedad > hmax && bomba_estado()) {
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

  // Publicar cambios de estado
  bool estado_bomba_actual = bomba_estado();

  if (estado_bomba_actual != estado_bomba_anterior) {
    mqtt_publicar_bomba(estado_bomba_actual);
    estado_bomba_anterior = estado_bomba_actual;
  }

  unsigned long ahora = millis();

  // OLED
  if (ahora - tiempo_oled >= intervalo_oled) {
    tiempo_oled = ahora;
    oled88_printRiego(humedad, modo_auto, bomba_estado());
  }

  // Serial info
  if (ahora - tiempo_reporte_serial >= intervalo_serial) {
    tiempo_reporte_serial = ahora;

    Serial.print("Soil humidity: ");
    Serial.print(humedad);
    Serial.println("%");

    Serial.print("Mode: ");
    Serial.println(modo_actual == MODO_AUTOMATICO ? "AUTO" : "MANUAL");

    Serial.print("Pump: ");
    Serial.println(bomba_estado() ? "ON" : "OFF");

    Serial.println("----------------------");
  }

  // 🔧 COMANDOS SERIAL
  procesar_comando_serial();
}
