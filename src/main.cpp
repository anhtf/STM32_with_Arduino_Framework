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
  g_stm32f407 = (stm32f407_impl_t *)(malloc(sizeof(stm32f407_impl_t)));
  g_stm32f407->m_usb_com_instance = communication_init(COMMUNICATION_BAURATE);
  
  Serial.write("--- Bat dau Setup ---\r\n", 23);

  g_stm32f407->m_spi_flash_instance = flash_spi_init(&spi1, PB0);
  Serial.write("... Khoi tao SPI Flash OK ...\r\n", 31);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  Serial.write("... Cau hinh chan LED OK ...\r\n", 30);
  
  Serial.write("... Bat dau xoa toan bo Flash ...\r\n", 36);
  g_stm32f407->m_spi_flash_instance->flash_spi_erase();
  Serial.write("... Xoa Flash thanh cong ...\r\n", 29);

  const char msg[] = "03112025_2108";
  Serial.write("... Bat dau ghi du lieu vao Flash ...\r\n", 38);
  g_stm32f407->m_spi_flash_instance->flash_spi_write(0x000000, (const uint8_t *)msg, sizeof(msg));
  Serial.write("... Ghi du lieu thanh cong ...\r\n", 31);
  
  Serial.write("--- Hoan thanh Setup ---\r\n", 26);
}

void loop()
{
  Serial.write("\r\n--- Bat dau Loop ---\r\n", 23);
  
  digitalToggle(LED_1_PIN);
  digitalToggle(LED_2_PIN);
  Serial.write("... Dao trang thai LED ...\r\n", 27);

  const char msg[] = "03112025_2108";
  uint8_t buf[32] = {0};
  Serial.write("... Doc du lieu tu Flash ...\r\n", 29);
  g_stm32f407->m_spi_flash_instance->flash_spi_read(0x000000, buf, sizeof(msg));
  
  Serial.write("... Doc thanh cong. Du lieu: ", 29);
  Serial.write((char *)buf, sizeof(msg));
  Serial.write("\r\n", 2);

  Serial.write("... Gui goi tin Healthcheck ...\r\n", 32);
  g_stm32f407->m_usb_com_instance->usb_comm_send((uint8_t *)&(g_stm32f407->m_usb_com_instance->m_healcheck_t), sizeof(mcu_healcheck_t));
  g_stm32f407->m_usb_com_instance->m_healcheck_t.m_miss_count++;
  
  Serial.write("--- Ket thuc Loop, doi 2s ---\r\n", 31);
  delay(2000);
}