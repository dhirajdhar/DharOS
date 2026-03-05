#ifndef ATA_H
#define ATA_H
#include <stdint.h>

// Initialize primary ATA bus, select master drive
void ata_init();

// Read/write a single 512-byte sector using LBA28
int ata_read_sector(uint32_t lba, uint8_t* buf);
int ata_write_sector(uint32_t lba, const uint8_t* buf);

#endif