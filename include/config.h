#ifndef CONFIG_H
#define CONFIG_H


/**
 * This constant define build options to the selected board
 * possible values:
 *  - NODEMCU
 *  - NANO
**/
//#define NODEMCU_BOARD 
#define NANO_BOARD

#ifdef NODEMCU_BOARD
    #define INPUT_PIN D1 //Button listen to actiavate
    #define OUTPUT_PIN LED_BUILTIN //Output to be activated on press
    #define RESET_PIN D2
#else
    #define INPUT_PIN 3 //Button listen to actiavate
    #define OUTPUT_PIN 2 //Output to be activated on press
    #define RESET_PIN 4
#endif

#define SERIAL_SPEED 115200
#define DEFAULT_ACTIVATION_TIME 3000 //IN MILISECONDS

#define ACTIVE_STATE LOW
#define INACTIVE_STATE HIGH

#define INPUT_ACTIVATE LOW

#define DEBUG false

const int EEPROM_TIME_ADDR = 00; //First position of EEMPROM
const int EEPROM_COUNT_ADDR = 10;

#endif