#include "../../intf/fs.h"
#include "../../intf/disk.h"
#include "../../intf/print.h"
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t  jmpBoot[3];
    uint8_t  OEMName[8];
    uint16_t BytesPerSector;
    uint8_t  SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t  NumFATs;
    uint16_t RootEntryCount;
    uint16_t TotalSectors16;
    uint8_t  Media;
    uint16_t FATSize16;
    uint16_t SectorsPerTrack;
    uint16_t NumHeads;
    uint32_t HiddenSectors;
    uint32_t TotalSectors32;

    uint32_t FATSize32;
    uint16_t ExtFlags;
    uint16_t FSVersion;
    uint32_t RootCluster;
    // ... rest omitted
} FAT32_BPB;
#pragma pack(pop)

static FAT32_BPB bpb;

// ----------------- Minimal helpers -----------------
static size_t str_len(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

static void str_copy_simple(char *dst, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) dst[i] = src[i];
    if (n > 0) dst[n - 1] = 0;
}

// ----------------- FS functions -----------------
void fs_init(void) {
    uint8_t sector[512];
    if (disk_read(0, sector) != 0) {
        print_str("FAT32: Disk read failed\n");
        return;
    }

    // copy BPB manually
    for (size_t i = 0; i < sizeof(FAT32_BPB); i++) {
        ((uint8_t *)&bpb)[i] = sector[i];
    }

    print_str("FAT32: Mounted successfully\n");
}

void fs_ls(const char *path) {
    uint8_t sector[512];
    uint32_t cluster = bpb.RootCluster;
    uint32_t lba = bpb.ReservedSectors + (bpb.NumFATs * bpb.FATSize32) +
                   ((cluster - 2) * bpb.SectorsPerCluster);

    if (disk_read(lba, sector) != 0) {
        print_str("FAT32: Read failed\n");
        return;
    }

    for (int i = 0; i < 512; i += 32) {
        char name[12];
        for (int j = 0; j < 11; j++) name[j] = sector[i + j];
        name[11] = 0;
        if (name[0] == 0x00) break;
        if (name[0] == 0xE5) continue;
        print_str(name);
        print_str("\n");
    }
}

int fs_mkdir(const char *path) {
    print_str("fs_mkdir not yet implemented: ");
    print_str(path);
    print_str("\n");
    return 0; // success placeholder
}

int fs_touch(const char *path) {
    print_str("fs_touch not yet implemented: ");
    print_str(path);
    print_str("\n");
    return 0; // success placeholder
}

int fs_read(const char *path, char *buf, size_t size) {
    print_str("fs_read not yet implemented: ");
    print_str(path);
    print_str("\n");
    if (buf && size > 0) buf[0] = 0;
    return 0;
}

int fs_write(const char *path, const char *data, size_t size) {
    print_str("fs_write not yet implemented: ");
    print_str(path);
    print_str("\n");

    // print data manually (no string.h)
    for (size_t i = 0; i < size; i++) {
        char c[2] = {data[i], 0};
        print_str(c);
    }

    print_str("\n");
    return (int)size; // pretend we wrote it
}
