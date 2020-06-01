#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include <Arduino.h>
#include "SerialInterface.cpp"

typedef struct serial_menu_entry serial_menu_entry_t;
typedef struct serial_menu serial_menu_t;

serial_menu_t* new_menu();

void add_menu_entry(serial_menu_t* menu, String label,bool (*function)());

void print_serial_menu(serial_menu_t* menu);

bool process_serial_request(serial_menu_t* menu);

#endif