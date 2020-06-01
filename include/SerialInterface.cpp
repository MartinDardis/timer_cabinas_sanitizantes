#include "SerialInterface.h"
#define INIT_MENU_ITEM 3

typedef struct serial_menu_entry {
    String label;
    bool (*function_ptr)();
} serial_menu_entry_t;

typedef struct serial_menu {
    int size;
    int capacity;
    serial_menu_entry_t** entries;
} serial_menu_t;

serial_menu_t* new_menu(){
  serial_menu_t* aux = (serial_menu_t*) calloc(1,sizeof(serial_menu_t));
  if (aux){
    aux->size = 0;
    aux->capacity = INIT_MENU_ITEM;
    aux->entries = (serial_menu_entry_t**) calloc(aux->capacity, sizeof(serial_menu_entry_t*));
    if (!aux->entries){
      free(aux);
      aux = NULL;
    }
  }
  return aux;
}

void add_menu_entry(serial_menu_t* menu, String label,bool (*function)()){
    if (menu->capacity == menu->size){
      int new_capacity = menu->capacity * 2;
      serial_menu_entry_t** aux = (serial_menu_entry_t**) realloc(menu->entries,new_capacity * sizeof(serial_menu_entry_t*));
      menu->entries = aux;
      menu->capacity = new_capacity;
    }
    serial_menu_entry_t* entry = (serial_menu_entry_t*) calloc(1,sizeof(serial_menu_entry_t));
    if (!entry)
      return;
    entry->label = label;
    entry->function_ptr = function;
    menu->entries[menu->size] = entry;
    menu->size ++;
    return;
}

void print_serial_menu(serial_menu_t* menu){
  Serial.println("Opciones:");
  for (int i = 0;i < menu->size; i++)
    Serial.println("[" + String(i,DEC) + "] " + menu->entries[i]->label);
}

bool process_serial_request(serial_menu_t* menu){
  String input_str = Serial.readString();
  int opt = input_str.toInt();
  if (opt >= menu->size || opt < 0){
    Serial.println("Opcion no valida");
    return false;
  }
  menu->entries[opt]->function_ptr();
  Serial.write(13);
  return true;
}