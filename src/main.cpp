#include <Arduino.h>
#include "config.h"
#ifdef NODEMCU_BOARD
  #include <ESP8266WiFi.h>
#endif
#include <EEPROM.h>
#include "SerialInterface.h"

unsigned int ACTIVATION_TIME = DEFAULT_ACTIVATION_TIME;
unsigned long TOTAL_COUNT = 0;
bool SERIAL_CONNECTED = false;

bool printConfig();
bool modif_tiempo_act();
bool doPurgado();
bool doHardReset();
bool doActivateAndCount();
bool printSystemInfo();

serial_menu_t* SERIAL_MENU;

void setup() {
  #ifdef NODEMCU_BOARD
    EEPROM.begin((size_t) 128);
  #else
    EEPROM.begin();
  #endif
  Serial.begin(SERIAL_SPEED);
  digitalWrite(OUTPUT_PIN,INACTIVE_STATE);
  pinMode(INPUT_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if (digitalRead(RESET_PIN) == INPUT_ACTIVATE)
    doHardReset();
  EEPROM.get(EEPROM_TIME_ADDR, ACTIVATION_TIME);
  EEPROM.get(EEPROM_COUNT_ADDR,TOTAL_COUNT);

  SERIAL_MENU= new_menu();
  add_menu_entry(SERIAL_MENU,"Ver parametros actual",printConfig);
  add_menu_entry(SERIAL_MENU,"Nueva activacion temporizada",doActivateAndCount);
  add_menu_entry(SERIAL_MENU,"Modif. tiempo de activacion",modif_tiempo_act);
  add_menu_entry(SERIAL_MENU,"Restablecer config de fabrica",doHardReset);
  add_menu_entry(SERIAL_MENU,"Purgado",doPurgado);
  if (DEBUG)
    add_menu_entry(SERIAL_MENU,"Info. sistema",printSystemInfo);

}

void loop() {
  if (digitalRead(INPUT_PIN) == INPUT_ACTIVATE)
    doActivateAndCount();
  if (Serial){
    if (!SERIAL_CONNECTED){
      print_serial_menu(SERIAL_MENU);
      SERIAL_CONNECTED = true;
    }
    if (Serial.available()){
      SERIAL_CONNECTED = !process_serial_request(SERIAL_MENU);
    }
  }else if(SERIAL_CONNECTED)
    SERIAL_CONNECTED = false;
}

bool doHardReset(){
    Serial.println("Restaurando...");
    EEPROM.put(EEPROM_TIME_ADDR,DEFAULT_ACTIVATION_TIME);
    EEPROM.put(EEPROM_COUNT_ADDR,0);
    ACTIVATION_TIME = DEFAULT_ACTIVATION_TIME;
    TOTAL_COUNT = 0;
    #ifdef NODEMCU_BOARD
      EEPROM.commit();
    #endif
    Serial.println("Finalizado.");
    return true;
}

bool doActivateAndCount(){
  Serial.println("Activacion en curso.");
  digitalWrite(OUTPUT_PIN,ACTIVE_STATE);
  delay( ACTIVATION_TIME);
  digitalWrite(OUTPUT_PIN,INACTIVE_STATE);
  TOTAL_COUNT ++;
  EEPROM.put(EEPROM_COUNT_ADDR,TOTAL_COUNT);
  #ifdef NODEMCU_BOARD
    EEPROM.commit();
  #endif
  Serial.println("Activacion finalizada");
  return true;
}

bool modif_tiempo_act(){
  Serial.println("Ingrese un nuevo tiempo de activacion (EN MILISEGUNDOS)");
  while (!Serial.available()){
    if (!Serial)
      return false;
  }
  String input = Serial.readString();
  if (input.length() < 1)
    return false;
  ACTIVATION_TIME = input.toInt();
  EEPROM.put(EEPROM_TIME_ADDR,ACTIVATION_TIME);
  #ifdef NODEMCU_BOARD
    EEPROM.commit();
  #endif
  Serial.println("Tiempo de activacion actualizado correctamente. ( " + input +" milisegundos )");
  return true;
}

bool doPurgado(){
  Serial.println("PURGANDO");
  digitalWrite(OUTPUT_PIN,ACTIVE_STATE);
  int state = HIGH;
  for(int i = 10; i != 0; i--){
    digitalWrite(LED_BUILTIN,state);
    state = state == HIGH? LOW : HIGH;
    Serial.print(String(i,DEC) + " ");
    delay(1000);
  }
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(OUTPUT_PIN,INACTIVE_STATE);
  Serial.println("FINALIZADO");
  return true;
}

bool printConfig (){
  Serial.println("Tiempo de activacion (miliSeg): " + String(ACTIVATION_TIME));
  Serial.println("Total activaciones: " + String(TOTAL_COUNT));
  return true;
}

bool printSystemInfo(){
  return true;
}