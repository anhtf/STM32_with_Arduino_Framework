#include <Arduino.h>
#include "comm.h"


#define LED_1_PIN PA6
#define LED_2_PIN PA7

extern packet_comm_t g_comm;

int myFunction(int, int);

void setup() {

  Serial.begin(115200);
  //while(!Serial);  
  Serial.write("STM32F407 USB CDC Ready!");

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

  int result = myFunction(2, 3);
}

void loop() {

  digitalToggle(LED_1_PIN);
  digitalToggle(LED_2_PIN);
  Serial.write((uint8_t*)&g_comm, sizeof(packet_comm_t));
  delay(5000);
}

int myFunction(int x, int y) {
  return x + y;
}