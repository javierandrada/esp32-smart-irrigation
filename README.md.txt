# Sistema de Riego Inteligente con ESP32

Proyecto desarrollado para la materia **Electrónica II – Sistemas Embebidos**
UTN FRRO.

El sistema mide la humedad del suelo y controla automáticamente una bomba de
riego. Además permite supervisión y control remoto mediante MQTT desde un
dispositivo móvil.

---

# Características

- Medición de humedad de suelo
- Control automático con histéresis
- Modo manual mediante pulsador
- Comunicación WiFi
- Comunicación MQTT
- Control remoto desde celular
- Visualización en display OLED
- Persistencia de configuración
- Configuración por puerto serie

---

# Hardware utilizado

- ESP32
- Sensor capacitivo de humedad de suelo
- Bomba de agua 5V
- Transistor TIP122
- Display OLED I2C
- Pulsadores de control
- LED Power ? indica alimentación del sistema
- LED WiFi ? indica conexión WiFi
- LED Pump ? indica activación de la bomba

---

# Arquitectura del firmware

El firmware está organizado en módulos independientes:
src/
main.ino

wifi.cpp

mqtt.cpp

sensor_humedad.cpp

bomba.cpp

pulsadores.cpp

oled.cpp

persist.cpp

Configuración del sistema:
include/

config_pin.h

config_oled.h

config_wifi.h
# Comunicación MQTT


Broker utilizado:



test.mosquitto.org

Topics utilizados:

Publicación:
riego/humedad

riego/bomba
Suscripción:



riego/comando



Comandos posibles:



ON

OFF



---

# Configuración por puerto serie


El sistema permite modificar parámetros desde el monitor serie.


Cambiar humedad mínima:



min 30



Cambiar humedad máxima:



max 60



Cambiar red WiFi:



ssid NombreDeRed

pass Contraseña



---

# Control automático


El sistema utiliza control con **histéresis**:


- Si humedad < humedad_min ? se activa la bomba

- Si humedad > humedad_max ? se desactiva la bomba


Esto evita encendidos y apagados repetitivos.


---

# Panel de control móvil


Se utilizó la aplicación **IoT MQTT Panel** para supervisión remota.


El panel muestra:


- Humedad del suelo

- Estado de la bomba

- Control ON/OFF


---

# Diagramas


Ver carpeta:



docs/



## Asignación de pines

| Dispositivo | GPIO ESP32 |
|--------------|-----------|
| Sensor humedad | GPIO34 |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| Pulsador MODE | GPIO18 |
| Pulsador MANUAL | GPIO19 |
| LED POWER | 3v3 |
| LED WIFI | GPIO 2|
| LED PUMP | GPIO 16 |
| Control bomba (TIP122) | GPIO5 |
---

# Autor


Proyecto desarrollado por:


**Javier Andrada**


UTN FRRO
Electrónica II – Sistemas Embebidos
