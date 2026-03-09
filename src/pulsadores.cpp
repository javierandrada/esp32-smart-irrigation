/**
 * @file pulsadores.cpp
 * @brief Push button management module
 *
 * Handles the MODE and MANUAL buttons used to control
 * the operating mode of the irrigation system.
 */
#include <Arduino.h>
#include "pulsadores.h"
#include "config_pins.h"
#include "bomba.h"

// System starts in automatic mode
bool modo_auto = true;

// Previous stable states
bool modo_anterior = HIGH;
bool manual_anterior = HIGH;

// Debounce timers
unsigned long ultimo_modo_ms = 0;
unsigned long ultimo_manual_ms = 0;

const unsigned long debounce_ms = 60;

void pulsadores_init() {
    pinMode(PIN_BOTON_MODO, INPUT_PULLUP);
    pinMode(PIN_BOTON_MANUAL, INPUT_PULLUP);
}

bool modo_automatico() {
    return modo_auto;
}

/**
 * @file pulsadores.cpp
 * @brief Push button management module
 *
 * Handles the MODE and MANUAL buttons used to control
 * the operating mode of the irrigation system.
 */
void pulsadores_loop() {
    unsigned long ahora = millis();

    bool modo_actual = digitalRead(PIN_BOTON_MODO);
    bool manual_actual = digitalRead(PIN_BOTON_MANUAL);

    // MODE button: detect press edge HIGH -> LOW
    if (modo_anterior == HIGH && modo_actual == LOW) {
        if (ahora - ultimo_modo_ms > debounce_ms) {
            modo_auto = !modo_auto;
            ultimo_modo_ms = ahora;

            if (modo_auto) {
                Serial.println("Mode changed to AUTOMATIC");
            } else {
                Serial.println("Mode changed to MANUAL");
                bomba_apagar();
            }
        }
    }

    // MANUAL button: only works in manual mode
    if (!modo_auto && manual_anterior == HIGH && manual_actual == LOW) {
        if (ahora - ultimo_manual_ms > debounce_ms) {
            ultimo_manual_ms = ahora;

            if (bomba_estado()) {
                bomba_apagar();
                Serial.println("Pump OFF by manual button");
            } else {
                bomba_encender();
                Serial.println("Pump ON by manual button");
            }
        }
    }

    modo_anterior = modo_actual;
    manual_anterior = manual_actual;
}
