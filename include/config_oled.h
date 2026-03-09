#ifndef _config_h_
#define _config_h_

//--Includes
#include <Arduino.h>


//--PCB UTN HARDWARE DEFINITIONS
//--Leds pins
#define ONBOARD_LED_PIN         2
#define LED1_PIN                38
#define LED2_PIN                1
#define LED3_PIN                2

//--Sensors pins
#define AN0_PIN                 5
#define AN1_PIN                 4
#define DHT22_PIN               20

//--I2C Pins
#define I2C_SDA_PIN             21
#define I2C_SCL_PIN             22

//--Switch pin
#define SWITCH_PIN              19

//-- Relays
#define RELAY1_PIN              21
#define RELAY2_PIN              14

//--Motor control
#define nRESET_PIN              13
#define nSLEEP_PIN              12
#define nFAULT_PIN              11
#define GPIO_DECAY_PIN          10
#define AIN1_PIN                8
#define AIN2_PIN                9
#define BIN1_PIN                18
#define BIN2_PIN                17
#define AI0_PIN                 16
#define AI1_PIN                 15
#define BI0_PIN                 7
#define BI1_PIN                 6

//--Software constants
#define SERIAL_BAUDRATE             19200

//--Structures
struct Config {
  float offset;
  float gain;
  uint8_t sensor;
};



//--Prototypes
void App_setup(void);
void App_loop(void);


#endif
