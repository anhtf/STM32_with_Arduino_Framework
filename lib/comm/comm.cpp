#include "comm.h"


static packet_send_t g_default_send_packet = {
    .m_header = 0x24,
    .m_data = 0,
    .m_crc = 0,
    .m_checksum = 0x24
};

static packet_recv_t g_default_recv_packet = 
{
    .m_header = 0x42,
    .m_request = 0x01,
    .m_data = 0,
    .m_crc = 0,
    .m_checksum = 0x43
};

static mcu_healcheck_t g_healcheck_packet = 
{
    .m_device_id = "25062003\0",
    .m_version   = "Version_1.1\0",
    .m_miss_count = 0
};

usb_comm_impl_t* communication_init(uint16_t _baurate)
{
    if (_baurate <= 0) return NULL;

    usb_comm_impl_t *usb_comm  = (usb_comm_impl_t*)malloc(sizeof(usb_comm_impl_t));

    usb_comm->m_baurate = _baurate;
    Serial.begin(_baurate);
    usb_comm->m_healcheck_t = g_healcheck_packet;
    usb_comm->m_send_t  = g_default_send_packet;
    usb_comm->m_recv_t  = g_default_recv_packet;
    usb_comm->usb_comm_send = send_data;

    return usb_comm;
}


void send_data(uint8_t *_data, uint16_t length)
{
    Serial.write(_data, length);
}

