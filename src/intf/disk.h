#ifndef DISK_H
#define DISK_H

#include <stdint.h>

#define SECTOR_SIZE 512

int disk_read(uint32_t lba, uint8_t *buffer);
int disk_write(uint32_t lba, const uint8_t *buffer);

#endif
