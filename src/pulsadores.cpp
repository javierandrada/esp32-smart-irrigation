/**
 * @file pulsadores.cpp
 * @brief Push button management module with software debounce
 *
 * This module handles the MODE and MANUAL buttons used to control
 * the irrigation system. A software debounce mechanism based on
 * millis() is implemented to filter mechanical bouncing.
 */

#include <Arduino.h>
#include "pulsadores.h"
#include "config_pins.h"
#include "bomba.h"

// System starts in automatic mode
bool modo_auto = true;

// Debounce time in milliseconds
const unsigned long debounce_ms = 60;

// Last instantaneous readings
bool lectura_modo_anterior = HIGH;
bool lectura_manual_anterior = HIGH;

// Stable button states after debounce validation
bool estado_modo = HIGH;
bool estado_manual = HIGH;

// Timestamp of the last detected change
unsigned long ultimo_cambio_modo = 0;
unsigned long ultimo_cambio_manual = 0;

void pulsadores_init() {
    pinMode(PIN_BOTON_MODO, INPUT_PULLUP);
    pinMode(PIN_BOTON_MANUAL, INPUT_PULLUP);
}

bool modo_automatico() {
    return modo_auto;
}

/**
 * @brief Periodically reads push buttons and applies debounce filtering
 *
 * The algorithm works as follows:
 * 1. Read the instantaneous button state
 * 2. Detect any change in the raw reading
 * 3. Wait until the state remains stable for debounce_ms
 * 4. Only then accept the change and execute the corresponding action
 */
void pulsadores_loop() {

    unsigned long ahora = millis();

    bool lectura_modo = digitalRead(PIN_BOTON_MODO);
    bool lectura_manual = digitalRead(PIN_BOTON_MANUAL);

    // -------- MODE BUTTON DEBOUNCE --------

    // If the instantaneous reading changed, reset debounce timer
    if (lectura_modo != lectura_modo_anterior) {
        ultimo_cambio_modo = ahora;
        lectura_modo_anterior = lectura_modo;
    }

    // If the state has been stable long enough, accept it
    if ((ahora - ultimo_cambio_modo) > debounce_ms) {

        if (lectura_modo != estado_modo) {
            estado_modo = lectura_modo;

            // Detect button press (falling edge HIGH -> LOW)
            if (estado_modo == LOW) {

                modo_auto = !modo_auto;

                if (modo_auto) {
                    Serial.println("Mode changed to AUTOMATIC");
                } else {
                    Serial.println("Mode changed to MANUAL");
                    bomba_apagar();
                }
            }
        }
    }

    // -------- MANUAL BUTTON DEBOUNCE --------

    if (lectura_manual != lectura_manual_anterior) {
        ultimo_cambio_manual = ahora;
        lectura_manual_anterior = lectura_manual;
    }

    if ((ahora - ultimo_cambio_manual) > debounce_ms) {

        if (lectura_manual != estado_manual) {
            estado_manual = lectura_manual;

            if (!modo_auto && estado_manual == LOW) {

                if (bomba_estado()) {
                    bomba_apagar();
                    Serial.println("Pump OFF by manual button");
                } else {
                    bomba_encender();
                    Serial.println("Pump ON by manual button");
                }
            }
        }
    }
}
