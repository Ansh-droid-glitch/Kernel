#include "../../intf/disk.h"
#include "../../intf/portio.h"  // You'll need outb/inb/outw/inw wrappers

#define ATA_PRIMARY_IO  0x1F0
#define ATA_PRIMARY_CTRL 0x3F6

static void ata_wait() {
    while (inb(ATA_PRIMARY_IO + 7) & 0x80); // wait until not busy
}

int disk_read(uint32_t lba, uint8_t *buffer) {
    ata_wait();
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F)); // master drive
    outb(ATA_PRIMARY_IO + 2, 1);                          // sector count
    outb(ATA_PRIMARY_IO + 3, lba & 0xFF);
    outb(ATA_PRIMARY_IO + 4, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_IO + 5, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_IO + 7, 0x20); // read command

    ata_wait();
    for (int i = 0; i < SECTOR_SIZE / 2; i++) {
        uint16_t data = inw(ATA_PRIMARY_IO);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8);
    }
    return 0;
}
