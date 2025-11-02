#pragma once

#include "Arduino.h"
#include "stdint.h"
#include "stdio.h"

#pragma pack(push,1)
typedef struct 
{
    uint8_t  m_header;
    uint8_t  m_request;
    uint8_t  m_data[64];
    uint16_t m_crc;
    uint8_t  m_checksum;
} packet_recv_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct 
{
    uint8_t  m_header;
    uint8_t  m_data[64];
    uint16_t m_crc;
    uint8_t  m_checksum;
} packet_send_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct 
{
    char     m_device_id[96];
    char     m_version[16];
    uint64_t m_miss_count;
} mcu_healcheck_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    int             m_baurate;
    mcu_healcheck_t m_healcheck_t;
    packet_recv_t   m_recv_t;
    packet_send_t   m_send_t;

    void (*usb_comm_send)(uint8_t *, uint16_t);
    void (*usb_comm_recv)(uint8_t *, uint16_t);
} usb_comm_impl_t;
#pragma pack(pop)


usb_comm_impl_t* communication_init(uint16_t _baurate);

void send_data(uint8_t *_data, uint16_t length);
