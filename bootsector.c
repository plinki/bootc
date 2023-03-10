#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bootsector.h"
#include "pbrfat.h"

#define BOOT_SECTOR_SIZE 512

void BootSector_init(BootSector* boot_sector, const uint8_t data[512]) {
    for (int i = 0; i < 512; i++) {
        boot_sector->data[i] = data[i];
    }
}

const uint8_t* data(const BootSector* boot_sector) {
    return boot_sector->data;
}

enum Type infer(const uint8_t* data) {
    if (data[510] == 0x55 && data[511] == 0xaa) {
        if ((data[0] == 0xeb && data[2] == 0x90) || data[0] == 0xe0) {
            return PBR_FAT;
        } else if (data[0x01be] == 0x00 || data[0x01be] == 0x80) {
            return MBR;
        }
    }
    return UNKNOWN;
}

BootSector* make_bs(const uint8_t* data, enum Type type) {
    BootSector* bs;
    switch(type) {
        case MBR: {
            bs = (BootSector*) malloc(sizeof(BootSector));
            // init?
            bs->Mbr_bs = (struct Mbr*) malloc(sizeof(struct Mbr));
            memcpy(bs->Mbr_bs, data, sizeof(struct Mbr));
            break;
        }
        case PBR_FAT: {
            bs = (BootSector*) malloc(sizeof(BootSector));
            bs->Pbr_bs = (struct PbrFat*) malloc(sizeof(struct PbrFat));
            PbrFat_init(bs->Pbr_bs, data);
            memcpy(bs->Pbr_bs, data, sizeof(struct PbrFat));
            break;
        }
        default:
            bs = (BootSector*) malloc(sizeof(BootSector));
            break;
    }
    return bs;
}

unsigned int cylinder(const uint8_t chs[3])
{
    return (unsigned int)((chs[1] & 0xc0u) << 2) | chs[2];
}

unsigned int head(const uint8_t chs[3])
{
    return chs[0];
}

unsigned int sector(const uint8_t chs[3])
{
    return (unsigned int)(chs[1] & 0x3fu);
}

