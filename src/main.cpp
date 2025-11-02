#include <Arduino.h>
#include "comm.h"
#include "spi_flash.h"

#define LED_1_PIN PA6
#define LED_2_PIN PA7

SPIClass spi1(PB5, PB4, PB3);

usb_comm_impl_t *usb_com;
flash_spi_impl  flash;
void setup() {
  
  usb_com = communication_init(115200);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

  flash_spi_init(&flash, &spi1, PB0);
  spi1.begin();

}

void loop() {

  digitalToggle(LED_1_PIN);
  digitalToggle(LED_2_PIN);
  usb_com->usb_comm_send((uint8_t *)&(usb_com->m_healcheck_t), sizeof(mcu_healcheck_t));

  flash.flash_spi_erase();  
  const char msg[] = "Hello anhtdh!";
  flash.flash_spi_write(0x000000, (const uint8_t*)msg, sizeof(msg));

  uint8_t buf[32] = {0};
  flash.flash_spi_read(0x000000, buf, sizeof(msg));

  Serial.print("Read back: ");
  Serial.println((char*)buf);
  delay(1000);
}



