#pragma once
#include <Arduino.h>
#include <SPI.h>

#define DEVICE_ID 0xEF4015

typedef struct 
{
    SPIClass *m_spi;                 
    uint32_t m_cs;                  

    void (*flash_spi_write)(uint32_t addr, const uint8_t *data, uint16_t len);
    void (*flash_spi_read)(uint32_t addr, uint8_t *data, uint16_t len);
    void (*flash_spi_erase)(void);
} flash_spi_impl;

//void flash_spi_init(flash_spi_impl *flash, SPIClass *spi, uint32_t cs);
flash_spi_impl* flash_spi_init (SPIClass *spi, uint32_t cs);