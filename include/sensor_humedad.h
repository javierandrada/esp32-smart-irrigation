#ifndef SENSOR_HUMEDAD_H
#define SENSOR_HUMEDAD_H

void sensor_humedad_init();
void sensor_humedad_loop();

int sensor_humedad_leer_raw();
int sensor_humedad_promedio();
int sensor_humedad_porcentaje();

bool sensor_humedad_nuevo_dato();

#endif
