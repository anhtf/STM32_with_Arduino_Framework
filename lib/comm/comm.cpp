#include "comm.h"

packet_comm_t g_comm;

bool communication_init(void)
{
    g_comm.m_header = 0x01;
}

