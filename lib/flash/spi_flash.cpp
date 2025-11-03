#include "spi_flash.h"

#define CMD_READ_DATA     0x03
#define CMD_PAGE_PROGRAM  0x02
#define CMD_WRITE_ENABLE  0x06
#define CMD_READ_STATUS   0x05
#define CMD_CHIP_ERASE    0xC7

static flash_spi_impl *g_flash = nullptr;


static void flash_select()   { digitalWrite(g_flash->m_cs, LOW); }
static void flash_deselect() { digitalWrite(g_flash->m_cs, HIGH); }

static void flash_write_enable()
{
    flash_select();
    g_flash->m_spi->transfer(CMD_WRITE_ENABLE);
    flash_deselect();
}

static void flash_wait_ready()
{
    uint8_t status;
    do {
        flash_select();
        g_flash->m_spi->transfer(CMD_READ_STATUS);
        status = g_flash->m_spi->transfer(0);
        flash_deselect();
        delayMicroseconds(100);
    } while (status & 0x01);
}


static void spi_flash_write_impl(uint32_t addr, const uint8_t *data, uint16_t len)
{
    flash_write_enable();
    flash_select();

    g_flash->m_spi->transfer(CMD_PAGE_PROGRAM);
    g_flash->m_spi->transfer((addr >> 16) & 0xFF);
    g_flash->m_spi->transfer((addr >> 8) & 0xFF);
    g_flash->m_spi->transfer(addr & 0xFF);

    for (uint16_t i = 0; i < len; i++)
        g_flash->m_spi->transfer(data[i]);

    flash_deselect();
    flash_wait_ready();
}

static void spi_flash_read_impl(uint32_t addr, uint8_t *data, uint16_t len)
{
    flash_select();

    g_flash->m_spi->transfer(CMD_READ_DATA);
    g_flash->m_spi->transfer((addr >> 16) & 0xFF);
    g_flash->m_spi->transfer((addr >> 8) & 0xFF);
    g_flash->m_spi->transfer(addr & 0xFF);

    for (uint16_t i = 0; i < len; i++)
        data[i] = g_flash->m_spi->transfer(0);

    flash_deselect();
}

static void spi_flash_erase_impl()
{
    flash_write_enable();
    flash_select();
    g_flash->m_spi->transfer(CMD_CHIP_ERASE);
    flash_deselect();
    flash_wait_ready();
}

flash_spi_impl* flash_spi_init (SPIClass *spi, uint32_t cs)
{
    if (spi == NULL || cs == 0) return NULL;

    flash_spi_impl* spi_obj = (flash_spi_impl*)(malloc(sizeof(flash_spi_impl)));
    if (spi_obj == NULL) return NULL;
    
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    spi->begin();

    spi_obj->m_spi = spi;
    spi_obj->m_cs = cs;

    spi_obj->flash_spi_write = spi_flash_write_impl;
    spi_obj->flash_spi_read = spi_flash_read_impl;
    spi_obj->flash_spi_erase = spi_flash_erase_impl;
    g_flash = spi_obj;
    return spi_obj;
}