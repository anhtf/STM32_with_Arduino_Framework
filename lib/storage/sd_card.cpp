#include "sd_card.h"
#include "utility/SdFat.h" 

struct SDClassMirror {
    Sd2Card card;
    SdVolume volume; 
    
};

static void sdcard_get_info_impl(void);
static bool sdcard_write_file_impl(const char *path, const uint8_t *data, uint16_t len);
static int sdcard_read_file_impl(const char *path, uint8_t *data, uint16_t len);


sdcard_impl_t* sdcard_init(void)
{
    Serial.write("[SDCARD] SD Card module init started.\r\n", 40);

    Serial.write("[SDCARD] Allocating memory for SD card instance...\r\n", 53);
    sdcard_impl_t* sd_obj = (sdcard_impl_t*)malloc(sizeof(sdcard_impl_t));
    if (sd_obj == NULL) {
        Serial.write("[SDCARD] Error: Memory allocation failed.\r\n", 43);
        return NULL;
    }
    Serial.write("[SDCARD] Memory allocated successfully.\r\n", 39);

    Serial.write("[SDCARD] Initializing SD card hardware (SDMMC)... \r\n", 52);
    if (!SD.begin()) {
        Serial.write("[SDCARD] Error: SD.begin() failed. Check wiring or card.\r\n", 62);
        sd_obj->m_is_initialized = false;
        free(sd_obj);
        return NULL;
    }
    
    Serial.write("[SDCARD] SD.begin() successful.\r\n", 33);
    sd_obj->m_is_initialized = true;

    Serial.write("[SDCARD] Assigning function pointers...\r\n", 40);
    sd_obj->sd_write_file = sdcard_write_file_impl;
    sd_obj->sd_read_file = sdcard_read_file_impl;
    sd_obj->sd_get_info = sdcard_get_info_impl;

    Serial.write("[SDCARD] Getting card information...\r\n", 37);
    sd_obj->sd_get_info();

    Serial.write("[SDCARD] SD Card module init finished successfully.\r\n", 54);
    return sd_obj;
}


static void sdcard_get_info_impl(void)
{
    Serial.write("[SDCARD-INFO] Card Type: SD/SDHC/SDXC detected.\r\n", 50);
    SDClassMirror* p_sd_mirror = (SDClassMirror*)&SD;

    uint32_t cluster_count = p_sd_mirror->volume.clusterCount();
    uint8_t blocks_per_cluster = p_sd_mirror->volume.blocksPerCluster();
    uint64_t cardSize = (uint64_t)cluster_count * blocks_per_cluster * 512 / (1024 * 1024);
    
    char sizeStr[40];
    sprintf(sizeStr, "[SDCARD-INFO] Card Size: %llu MB\r\n", cardSize);
    Serial.write(sizeStr, strlen(sizeStr));
}

static bool sdcard_write_file_impl(const char *path, const uint8_t *data, uint16_t len)
{
    char log_buf[128];
    sprintf(log_buf, "[SDCARD-IO] Attempting to open file '%s' for writing...\r\n", path);
    Serial.write(log_buf, strlen(log_buf));

    File dataFile = SD.open(path, FILE_WRITE);
    if (!dataFile) {
        sprintf(log_buf, "[SDCARD-IO] Error: Failed to open '%s' for writing.\r\n", path);
        Serial.write(log_buf, strlen(log_buf));
        return false;
    }

    sprintf(log_buf, "[SDCARD-IO] Writing %d bytes to '%s'...\r\n", len, path);
    Serial.write(log_buf, strlen(log_buf));
    
    size_t written_bytes = dataFile.write(data, len);
    dataFile.close();

    if (written_bytes == len) {
        sprintf(log_buf, "[SDCARD-IO] Write to '%s' successful.\r\n", path);
        Serial.write(log_buf, strlen(log_buf));
        return true;
    } else {
        sprintf(log_buf, "[SDCARD-IO] Error: Write failed. Wrote %d of %d bytes.\r\n", written_bytes, len);
        Serial.write(log_buf, strlen(log_buf));
        return false;
    }
}


static int sdcard_read_file_impl(const char *path, uint8_t *data, uint16_t len)
{
    char log_buf[128];
    sprintf(log_buf, "[SDCARD-IO] Attempting to open file '%s' for reading...\r\n", path);
    Serial.write(log_buf, strlen(log_buf));

    File dataFile = SD.open(path);
    if (!dataFile) {
        sprintf(log_buf, "[SDCARD-IO] Error: Failed to open '%s' for reading.\r\n", path);
        Serial.write(log_buf, strlen(log_buf));
        return -1;
    }

    sprintf(log_buf, "[SDCARD-IO] Reading up to %d bytes from '%s'...\r\n", len, path);
    Serial.write(log_buf, strlen(log_buf));

    int bytes_read = dataFile.read(data, len);
    dataFile.close();

    if (bytes_read >= 0) {
        sprintf(log_buf, "[SDCARD-IO] Read %d bytes from '%s' successfully.\r\n", bytes_read, path);
        Serial.write(log_buf, strlen(log_buf));
    } else {
        sprintf(log_buf, "[SDCARD-IO] Error: Read failed for file '%s'.\r\n", path);
        Serial.write(log_buf, strlen(log_buf));
    }
    
    return bytes_read;
}