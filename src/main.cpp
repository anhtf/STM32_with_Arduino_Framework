#include <Arduino.h>
#include "comm.h"
#include "spi_flash.h"
#include "sd_card.h" 

#define LED_1_PIN PA6
#define LED_2_PIN PA7

#define COMMUNICATION_BAURATE 115200

SPIClass spi1(PB5, PB4, PB3);

typedef struct stm32f407_impl
{
  usb_comm_impl_t *m_usb_com_instance;
  flash_spi_impl  *m_spi_flash_instance;
  sdcard_impl_t   *m_sdcard_instance; // Add a member for the SD card instance
} stm32f407_impl_t;


stm32f407_impl_t *g_stm32f407;

void setup()
{
  g_stm32f407 = (stm32f407_impl_t *)(malloc(sizeof(stm32f407_impl_t)));
  
  g_stm32f407->m_usb_com_instance = communication_init(COMMUNICATION_BAURATE);
  
  Serial.write("\r\n--- Main Setup Started ---\r\n", 29);

  Serial.write("[MAIN] Calling SPI Flash initializer...\r\n", 39);
  g_stm32f407->m_spi_flash_instance = flash_spi_init(&spi1, PB0);
  
  // Initialize the SD Card driver
  Serial.write("[MAIN] Calling SD Card initializer...\r\n", 37);
  g_stm32f407->m_sdcard_instance = sdcard_init();

  Serial.write("[MAIN] Configuring LED pins...\r\n", 32);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  Serial.write("[MAIN] LED pins configured.\r\n", 29);
  
  Serial.write("[MAIN] Performing full chip erase on SPI Flash...\r\n", 51);
  g_stm32f407->m_spi_flash_instance->flash_spi_erase();
  Serial.write("[MAIN] Chip erase successful.\r\n", 30);

  const char flash_msg[] = "03112025_2108_FLASH";
  Serial.write("[MAIN] Writing data to SPI flash...\r\n", 37);
  g_stm32f407->m_spi_flash_instance->flash_spi_write(0x000000, (const uint8_t *)flash_msg, sizeof(flash_msg));
  Serial.write("[MAIN] Data write to SPI flash successful.\r\n", 44);
  
  Serial.write("--- Main Setup Finished ---\r\n", 28);
}

void loop()
{
  Serial.write("\r\n--- Main Loop Cycle Start ---\r\n", 32);
  
  digitalToggle(LED_1_PIN);
  digitalToggle(LED_2_PIN);
  Serial.write("[MAIN] Toggled LEDs.\r\n", 22);

  // --- SD Card Operation ---
  if (g_stm32f407->m_sdcard_instance && g_stm32f407->m_sdcard_instance->m_is_initialized) {
    const char *filename = "/log.txt";
    const char sd_msg[] = "Hello from STM32 SD Card!";
    g_stm32f407->m_sdcard_instance->sd_write_file(filename, (const uint8_t*)sd_msg, sizeof(sd_msg));
    
    uint8_t sd_buf[64] = {0};
    int bytes_read = g_stm32f407->m_sdcard_instance->sd_read_file(filename, sd_buf, sizeof(sd_msg));

    if (bytes_read > 0) {
      Serial.write("[MAIN] SD Read successful. Data: ", 33);
      Serial.write((char *)sd_buf, bytes_read);
      Serial.write("\r\n", 2);
    }
  } else {
    Serial.write("[MAIN] SD Card not initialized, skipping operations.\r\n", 56);
  }

  // --- SPI Flash Operation ---
  const char flash_msg[] = "03112025_2108_FLASH";
  uint8_t flash_buf[32] = {0};
  Serial.write("[MAIN] Reading data from SPI flash...\r\n", 39);
  g_stm32f407->m_spi_flash_instance->flash_spi_read(0x000000, flash_buf, sizeof(flash_msg));
  
  Serial.write("[MAIN] SPI Read successful. Data: ", 34);
  Serial.write((char *)flash_buf, sizeof(flash_msg));
  Serial.write("\r\n", 2);

  // --- Communication Operation ---
  Serial.write("[MAIN] Sending Healthcheck packet...\r\n", 37);
  g_stm32f407->m_usb_com_instance->usb_comm_send((uint8_t *)&(g_stm32f407->m_usb_com_instance->m_healcheck_t), sizeof(mcu_healcheck_t));
  g_stm32f407->m_usb_com_instance->m_healcheck_t.m_miss_count++;
  
  Serial.write("--- Main Loop Cycle End, delaying 2s ---\r\n", 42);
  delay(2000);
}