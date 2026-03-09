/**
 * @file sensor_humedad.cpp
 * @brief Soil humidity sensor module
 *
 * Reads the capacitive soil moisture sensor connected to the ESP32
 * analog input and converts the value into a percentage.
 *
 * The module also performs averaging to reduce noise and provide
 * stable humidity readings for the irrigation control algorithm.
 */
#include <Arduino.h>
#include "sensor_humedad.h"
#include "config_pins.h"

// Number of samples for averaging
#define NUM_MUESTRAS 10

// Calibration values
#define HUMEDAD_SECO 3000
#define HUMEDAD_MOJADO 1313

// Time between samples (non-blocking)
const unsigned long intervalo_muestra = 50;

// Internal variables for non-blocking averaging
unsigned long ultimo_tiempo_muestra = 0;

int suma_muestras = 0;
int contador_muestras = 0;

int valor_minimo = 4095;
int valor_maximo = 0;

int ultimo_raw = 0;
int ultimo_promedio = 0;
int ultimo_porcentaje = 0;

bool dato_nuevo = false;

void sensor_humedad_init() {

    pinMode(PIN_SENSOR_HUMEDAD, INPUT);

    ultimo_tiempo_muestra = millis();
}

// Read a single raw analog value
int sensor_humedad_leer_raw() {

    ultimo_raw = analogRead(PIN_SENSOR_HUMEDAD);
    return ultimo_raw;
}

// Non-blocking sampling task
void sensor_humedad_loop() {

    unsigned long ahora = millis();

    if (ahora - ultimo_tiempo_muestra >= intervalo_muestra) {

        ultimo_tiempo_muestra = ahora;

        int lectura = sensor_humedad_leer_raw();

        suma_muestras += lectura;
        contador_muestras++;

        if (lectura < valor_minimo) {
            valor_minimo = lectura;
        }

        if (lectura > valor_maximo) {
            valor_maximo = lectura;
        }

        // When enough samples are collected, calculate average and percentage
        if (contador_muestras >= NUM_MUESTRAS) {

            ultimo_promedio = suma_muestras / NUM_MUESTRAS;

            ultimo_porcentaje = map(ultimo_promedio, HUMEDAD_SECO, HUMEDAD_MOJADO, 0, 100);
            ultimo_porcentaje = constrain(ultimo_porcentaje, 0, 100);

            // Diagnostic information
            Serial.print("Raw Min: ");
            Serial.print(valor_minimo);

            Serial.print(" Raw Max: ");
            Serial.print(valor_maximo);

            Serial.print(" Raw Avg: ");
            Serial.print(ultimo_promedio);

            Serial.print(" Humedad: ");
            Serial.print(ultimo_porcentaje);
            Serial.println("%");

            // Reset accumulators for next cycle
            suma_muestras = 0;
            contador_muestras = 0;
            valor_minimo = 4095;
            valor_maximo = 0;

            dato_nuevo = true;
        }
    }
}

// Returns last calculated average
int sensor_humedad_promedio() {

    return ultimo_promedio;
}

// Returns last calculated humidity percentage
int sensor_humedad_porcentaje() {

    return ultimo_porcentaje;
}

// Returns true only once when a new averaged value is available
bool sensor_humedad_nuevo_dato() {

    if (dato_nuevo) {
        dato_nuevo = false;
        return true;
    }

    return false;
}
