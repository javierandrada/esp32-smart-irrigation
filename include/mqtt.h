#ifndef MQTT_H
#define MQTT_H

void mqtt_init();
void mqtt_loop();
void mqtt_publicar_humedad(float humedad);
void mqtt_publicar_bomba(bool estado);

bool mqtt_hay_comando();
bool mqtt_comando_encender();
bool mqtt_comando_apagar();
void mqtt_limpiar_comando();

#endif
