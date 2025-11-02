#pragma once

#include "stdint.h"
#include "stdio.h"

#pragma pack(push,1)
typedef struct 
{
    uint8_t m_header;
    uint8_t m_request;
    uint8_t m_data[64];
    uint16_t crc;
    uint8_t checksum;
} packet_comm_t;
#pragma pack(pop)

bool communication_init(void);