/**
 * @file bomba.cpp
 * @brief Irrigation pump control module
 *
 * Controls the water pump using a transistor (TIP122)
 * connected to a GPIO pin of the ESP32.
 */
#include <Arduino.h>
#include "bomba.h"
#include "config_pins.h"

// Humidity thresholds
#define HUMEDAD_MIN 55
#define HUMEDAD_MAX 70

bool estado_bomba = false;

/**
 * @brief Initializes pump control pin
 */
void bomba_init() {

    pinMode(PIN_BOMBA, OUTPUT);
    pinMode(PIN_LED_PUMP, OUTPUT);

    digitalWrite(PIN_BOMBA, LOW);
    digitalWrite(PIN_LED_PUMP, LOW);
}

// Turn pump ON
void bomba_encender() {

    digitalWrite(PIN_BOMBA, HIGH);
    digitalWrite(PIN_LED_PUMP, HIGH);

    estado_bomba = true;

    Serial.println("Riego ACTIVADO");
}

// Turn pump OFF
void bomba_apagar() {

    digitalWrite(PIN_BOMBA, LOW);
    digitalWrite(PIN_LED_PUMP, LOW);

    estado_bomba = false;

    Serial.println("Riego DESACTIVADO");
}

// Automatic irrigation control
void bomba_controlar(int humedad) {

    if (humedad < HUMEDAD_MIN && !estado_bomba) {

        bomba_encender();
    }

    if (humedad > HUMEDAD_MAX && estado_bomba) {

        bomba_apagar();
    }
}

// ESTA FUNCION VA FUERA DE TODO
bool bomba_estado() {

    return estado_bomba;
}
