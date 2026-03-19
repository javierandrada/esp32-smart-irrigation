/**
 * @file mqtt.cpp
 * @brief MQTT communication module for the irrigation system
 *
 * This module manages the MQTT connection with the broker,
 * publishes system data (soil humidity and pump state),
 * and receives remote commands to control the irrigation pump.
 *
 * Broker: test.mosquitto.org
 * Topics used:
 *   Publish:
 *      riego/humedad
 *      riego/bomba
 *   Subscribe:
 *      riego/comando
 */
#include "mqtt.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "test.mosquitto.org";

// Variables to save as received
bool comando_pendiente = false;
bool comando_on = false;
bool comando_off = false;

// Reconnection control
unsigned long mqtt_reconnect_timer = 0;
const unsigned long mqtt_reconnect_interval = 5000;

/**
 * @brief MQTT message callback
 *
 * This function is called whenever a subscribed topic
 * receives a new message. It processes the command
 * received from the MQTT broker.
 */
void callback(char* topic, byte* payload, unsigned int length) {

  String mensaje = "";

  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.print("MQTT -> Mensaje recibido en topic: ");
  Serial.println(topic);

  Serial.print("MQTT -> Contenido: ");
  Serial.println(mensaje);

  if (String(topic) == "riego/comando") {
    if (mensaje == "ON") {
      comando_pendiente = true;
      comando_on = true;
      comando_off = false;
    }
    else if (mensaje == "OFF") {
      comando_pendiente = true;
      comando_on = false;
      comando_off = true;
    }
  }
}

/**
 * @brief Attempts to reconnect to the MQTT broker
 *
 * Non-blocking reconnection:
 * tries to reconnect every 5 seconds if disconnected.
 */
void mqtt_reconnect() {

  if (client.connected()) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  unsigned long ahora = millis();

  if (ahora - mqtt_reconnect_timer >= mqtt_reconnect_interval) {
    mqtt_reconnect_timer = ahora;

    Serial.print("Connecting to MQTT...");

    if (client.connect("ESP32_RIEGO")) {
      Serial.println("connected");
      client.subscribe("riego/comando");
      Serial.println("MQTT -> Suscripto a riego/comando");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
}

void mqtt_init() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void mqtt_loop() {

  if (!client.connected()) {
    mqtt_reconnect();
    return;
  }

  client.loop();
}

/**
 * @brief Publishes soil humidity value to MQTT broker
 */
void mqtt_publicar_humedad(float humedad) {

  if (!client.connected()) return;

  char mensaje[10];
  dtostrf(humedad, 1, 2, mensaje);

  client.publish("riego/humedad", mensaje);

  Serial.print("MQTT -> Humedad publicada: ");
  Serial.println(mensaje);
}

/**
 * @brief Publishes pump state (ON/OFF) to MQTT broker
 */
void mqtt_publicar_bomba(bool estado) {

  if (!client.connected()) return;

  if (estado) {
    client.publish("riego/bomba", "ON");
    Serial.println("MQTT -> Bomba publicada: ON");
  } else {
    client.publish("riego/bomba", "OFF");
    Serial.println("MQTT -> Bomba publicada: OFF");
  }
}

bool mqtt_hay_comando() {
  return comando_pendiente;
}

bool mqtt_comando_encender() {
  return comando_on;
}

bool mqtt_comando_apagar() {
  return comando_off;
}

void mqtt_limpiar_comando() {
  comando_pendiente = false;
  comando_on = false;
  comando_off = false;
}
