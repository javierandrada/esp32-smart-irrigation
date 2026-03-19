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

// Pump status function
bool bomba_estado() {

    return estado_bomba;
}

