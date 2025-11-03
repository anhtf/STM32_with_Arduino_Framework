#include <Arduino.h>
#include "comm.h"
#include "spi_flash.h"

#define LED_1_PIN PA6
#define LED_2_PIN PA7

#define COMMUNICATION_BAURATE 115200

SPIClass spi1(PB5, PB4, PB3);

typedef struct stm32f407_impl
{
  usb_comm_impl_t *m_usb_com_instance;
  flash_spi_impl *m_spi_flash_instance;
} stm32f407_impl_t;


stm32f407_impl_t *g_stm32f407;

void setup()
{
  delay (5000);
  g_stm32f407 = (stm32f407_impl_t *)(malloc(sizeof(stm32f407_impl_t)));
  
  g_stm32f407->m_usb_com_instance = communication_init(COMMUNICATION_BAURATE);
  
  Serial.write("\r\n--- Main Setup Started ---\r\n", 29);

  Serial.write("[MAIN] Calling SPI Flash initializer...\r\n", 39);
  g_stm32f407->m_spi_flash_instance = flash_spi_init(&spi1, PB0);

  Serial.write("[MAIN] Configuring LED pins...\r\n", 32);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  Serial.write("[MAIN] LED pins configured.\r\n", 29);
  
  Serial.write("[MAIN] Performing full chip erase...\r\n", 37);
  g_stm32f407->m_spi_flash_instance->flash_spi_erase();
  Serial.write("[MAIN] Chip erase successful.\r\n", 30);

  const char msg[] = "03112025_2108";
  Serial.write("[MAIN] Writing data to flash...\r\n", 33);
  g_stm32f407->m_spi_flash_instance->flash_spi_write(0x000000, (const uint8_t *)msg, sizeof(msg));
  Serial.write("[MAIN] Data write successful.\r\n", 31);
  
  Serial.write("--- Main Setup Finished ---\r\n", 28);
}

void loop()
{
  Serial.write("\r\n--- Main Loop Cycle Start ---\r\n", 32);
  
  digitalToggle(LED_1_PIN);
  digitalToggle(LED_2_PIN);
  Serial.write("[MAIN] Toggled LEDs.\r\n", 22);

  const char msg[] = "03112025_2108";
  uint8_t buf[32] = {0};
  Serial.write("[MAIN] Reading data from flash...\r\n", 35);
  g_stm32f407->m_spi_flash_instance->flash_spi_read(0x000000, buf, sizeof(msg));
  
  Serial.write("[MAIN] Read successful. Data: ", 30);
  Serial.write((char *)buf, sizeof(msg));
  Serial.write("\r\n", 2);

  Serial.write("[MAIN] Sending Healthcheck packet...\r\n", 37);
  g_stm32f407->m_usb_com_instance->usb_comm_send((uint8_t *)&(g_stm32f407->m_usb_com_instance->m_healcheck_t), sizeof(mcu_healcheck_t));
  g_stm32f407->m_usb_com_instance->m_healcheck_t.m_miss_count++;
  
  Serial.write("--- Main Loop Cycle End, delaying 2s ---\r\n", 42);
  delay(2000);
}