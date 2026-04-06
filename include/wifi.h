#ifndef WIFI_H
#define WIFI_H

extern bool wifi_conectado;

void wifi_init();
void wifi_loop();
void wifi_reconnect_now();

#endif
