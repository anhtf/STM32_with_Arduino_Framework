#include "comm.h"

static packet_send_t g_default_send_packet = {
    .m_header = 0x24,
    .m_data = {0},
    .m_crc = 0,
    .m_checksum = 0x24
};

static packet_recv_t g_default_recv_packet = 
{
    .m_header = 0x42,
    .m_request = 0x01,
    .m_data = {0},
    .m_crc = 0,
    .m_checksum = 0x43
};

static mcu_healcheck_t g_healcheck_packet = 
{
    .m_device_id = "25062003\0",
    .m_version   = "Version_1.1\0",
    .m_miss_count = 0
};

// Changed parameter from uint16_t to uint32_t
usb_comm_impl_t* communication_init(uint32_t _baurate)
{
    if (_baurate > 0) {
        Serial.begin(_baurate);
    }

    Serial.write("[COMM] Communication module init started.\r\n", 42);

    if (_baurate <= 0) {
        Serial.write("[COMM] Error: Invalid baud rate provided.\r\n", 44);
        return NULL;
    }

    Serial.write("[COMM] Allocating memory for communication instance...\r\n", 57);
    usb_comm_impl_t *usb_comm = (usb_comm_impl_t*)malloc(sizeof(usb_comm_impl_t));
    if (usb_comm == NULL) {
        Serial.write("[COMM] Error: Memory allocation failed.\r\n", 41);
        return NULL;
    }
    Serial.write("[COMM] Memory allocated successfully.\r\n", 37);

    usb_comm->m_baurate = (int)_baurate;
    Serial.write("[COMM] Baud rate set.\r\n", 23);

    Serial.write("[COMM] Setting up default healthcheck packet...\r\n", 48);
    usb_comm->m_healcheck_t = g_healcheck_packet;
    Serial.write("[COMM] Setting up default send packet...\r\n", 40);
    usb_comm->m_send_t = g_default_send_packet;
    Serial.write("[COMM] Setting up default receive packet...\r\n", 43);
    usb_comm->m_recv_t = g_default_recv_packet;

    Serial.write("[COMM] Assigning send function pointer...\r\n", 42);
    usb_comm->usb_comm_send = send_data;

    Serial.write("[COMM] Communication module init finished successfully.\r\n", 57);
    return usb_comm;
}


void send_data(uint8_t *_data, uint16_t length)
{
    Serial.write(_data, length);
}