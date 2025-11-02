#include <Arduino.h>
#include "comm.h"


#define LED_1_PIN PA6
#define LED_2_PIN PA7


usb_comm_impl_t *usb_com;

void setup() {
  
  usb_com = communication_init(115200);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

}

void loop() {

  digitalToggle(LED_1_PIN);
  digitalToggle(LED_2_PIN);
  usb_com->usb_comm_send((uint8_t *)&(usb_com->m_healcheck_t), sizeof(mcu_healcheck_t));
  delay(1000);
}

int myFunction(int x, int y) {
  return x + y;
}