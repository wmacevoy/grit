#pragma once

#include <stddef.h>
#include <stdint.h>

extern const uint16_t CRC16_table[256];

#define CRC16_INIT 0xFFFF
#define CRC16_NEXT(crcval,newchar) ((crcval) = ((crcval) >> 8) ^ CRC16_table[((crcval) ^ (newchar)) & 0x00ff])

uint16_t CRC16(const void *message, size_t length);
