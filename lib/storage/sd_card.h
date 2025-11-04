#pragma once

#include <Arduino.h>
#include <SD.h>

typedef struct
{
    bool m_is_initialized;

    bool (*sd_write_file)(const char *path, const uint8_t *data, uint16_t len);
    int (*sd_read_file)(const char *path, uint8_t *data, uint16_t len);
    void (*sd_get_info)(void);

} sdcard_impl_t;

sdcard_impl_t* sdcard_init(void);