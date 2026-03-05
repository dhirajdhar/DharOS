#include "ata.h"
#include "io.h"

// Primary bus ports
#define ATA_IO_BASE      0x1F0
#define ATA_REG_DATA     (ATA_IO_BASE + 0)
#define ATA_REG_ERROR    (ATA_IO_BASE + 1)
#define ATA_REG_FEATURES (ATA_IO_BASE + 1)
#define ATA_REG_SECTCNT  (ATA_IO_BASE + 2)
#define ATA_REG_LBA0     (ATA_IO_BASE + 3)
#define ATA_REG_LBA1     (ATA_IO_BASE + 4)
#define ATA_REG_LBA2     (ATA_IO_BASE + 5)
#define ATA_REG_HDDEVSEL (ATA_IO_BASE + 6)
#define ATA_REG_COMMAND  (ATA_IO_BASE + 7)
#define ATA_REG_STATUS   (ATA_IO_BASE + 7)

// Control ports
#define ATA_CTRL_BASE    0x3F6
#define ATA_REG_ALTSTAT  (ATA_CTRL_BASE + 0)
#define ATA_REG_DEVCTRL  (ATA_CTRL_BASE + 0)

#define ATA_CMD_READ_SECT    0x20
#define ATA_CMD_WRITE_SECT   0x30

#define ATA_SR_ERR   0x01
#define ATA_SR_DRQ   0x08
#define ATA_SR_DF    0x20
#define ATA_SR_DRDY  0x40
#define ATA_SR_BSY   0x80

static void ata_delay400ns() {
    inb(ATA_REG_ALTSTAT); inb(ATA_REG_ALTSTAT);
    inb(ATA_REG_ALTSTAT); inb(ATA_REG_ALTSTAT);
}

static int ata_wait_busy_clear() {
    uint32_t timeout = 1000000;
    while (timeout--) {
        uint8_t st = inb(ATA_REG_STATUS);
        if (!(st & ATA_SR_BSY)) return 0;
    }
    return -1;
}

static int ata_wait_drq() {
    uint32_t timeout = 1000000;
    while (timeout--) {
        uint8_t st = inb(ATA_REG_STATUS);
        if (st & ATA_SR_ERR) return -2;
        if (st & ATA_SR_DF)  return -3;
        if (!(st & ATA_SR_BSY) && (st & ATA_SR_DRQ)) return 0;
    }
    return -4;
}

void ata_init() {
    // Select master drive, LBA mode
    outb(ATA_REG_HDDEVSEL, 0xE0); // 1110 0000: LBA master
    ata_delay400ns();
    // Optionally: IDENTIFY (0xEC) could be issued here; we keep it minimal.
}

int ata_read_sector(uint32_t lba, uint8_t* buf) {
    if (ata_wait_busy_clear() != 0) return -1;

    outb(ATA_REG_DEVCTRL, 0x00); // enable interrupts (optional)
    outb(ATA_REG_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F)); // LBA high nibble + master

    outb(ATA_REG_SECTCNT, 1);
    outb(ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outb(ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));

    outb(ATA_REG_COMMAND, ATA_CMD_READ_SECT);

    if (ata_wait_drq() != 0) return -2;

    // Read 256 words = 512 bytes
    for (int i = 0; i < 256; i++) {
        uint16_t w = inw(ATA_REG_DATA);
        buf[i * 2 + 0] = (uint8_t)(w & 0xFF);
        buf[i * 2 + 1] = (uint8_t)(w >> 8);
    }
    ata_delay400ns();
    return 0;
}

int ata_write_sector(uint32_t lba, const uint8_t* buf) {
    if (ata_wait_busy_clear() != 0) return -1;

    outb(ATA_REG_DEVCTRL, 0x00);
    outb(ATA_REG_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));

    outb(ATA_REG_SECTCNT, 1);
    outb(ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outb(ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));

    outb(ATA_REG_COMMAND, ATA_CMD_WRITE_SECT);

    if (ata_wait_drq() != 0) return -2;

    // Write 256 words = 512 bytes
    for (int i = 0; i < 256; i++) {
        uint16_t w = (uint16_t)buf[i * 2 + 0] | ((uint16_t)buf[i * 2 + 1] << 8);
        outw(ATA_REG_DATA, w);
    }

    // Flush by polling for BSY clear and DRQ clear
    ata_delay400ns();

    // Check final status for error
    uint8_t st = inb(ATA_REG_STATUS);
    if (st & (ATA_SR_ERR | ATA_SR_DF)) return -3;
    return 0;
}