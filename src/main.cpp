#include <Arduino.h>
#include "config.h"
#ifdef NODEMCU_BOARD
  #include <ESP8266WiFi.h>
#endif
#include <EEPROM.h>
#include "SerialInterface.h"

unsigned long TOTAL_COUNT = 0;
bool SERIAL_CONNECTED = false;
bool running = false;
unsigned long startMillis = 0;

bool printConfig();
bool doPurgado();
bool doActivateAndCount();
bool doHardReset();
bool isReallyButtonPressed();

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

  if (digitalRead(RESET_PIN) == INPUT_ACTIVATE){
    doHardReset();
  }
  EEPROM.get(EEPROM_COUNT_ADDR,TOTAL_COUNT);

  SERIAL_MENU= new_menu();
  add_menu_entry(SERIAL_MENU,"Ver parametros actual",printConfig);
  add_menu_entry(SERIAL_MENU,"Restablecer config de fabrica",doHardReset);
  add_menu_entry(SERIAL_MENU,"Purgado",doPurgado);
}

void loop() {
  if (digitalRead(INPUT_PIN) == INPUT_ACTIVATE){
    if ( isReallyButtonPressed() ){
      doActivateAndCount(); 
    }
  }
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
    EEPROM.put(EEPROM_COUNT_ADDR,0);
    TOTAL_COUNT = 0;
    #ifdef NODEMCU_BOARD
      EEPROM.commit();
    #endif
    Serial.println("Finalizado.");
    return true;
}

bool doActivateAndCount(){
  Serial.println("Activacion en curso.");
  if ( ! running ){
    digitalWrite(OUTPUT_PIN,ACTIVE_STATE);
    startMillis = millis();
    running = true;
    digitalWrite(LED_BUILTIN,HIGH);
  } else {
    digitalWrite(OUTPUT_PIN,INACTIVE_STATE);
    running = false;
    long elapsed = ( millis() - startMillis ) / 1000;
    TOTAL_COUNT = TOTAL_COUNT + elapsed;
    EEPROM.put(EEPROM_COUNT_ADDR,TOTAL_COUNT);
    digitalWrite(LED_BUILTIN,LOW);
  }
  #ifdef NODEMCU_BOARD
    EEPROM.commit();
  #endif
  Serial.println("Activacion finalizada");
  delay(PUSH_DELAY_MS);
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
  Serial.println("Tiempo total activado (seg): " + String(TOTAL_COUNT));
  return true;
}

bool isReallyButtonPressed(){
  bool result = true;
  for(int i=0; i < 10; i++ ){
    result = result && ( digitalRead(INPUT_PIN) == INPUT_ACTIVATE );
    delay(50);
  }
  return result;
}