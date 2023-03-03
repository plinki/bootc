#include <stdio.h>
#include <stdint.h>
#include "bootsector.h"
#include "read.h"

void bytes(uint8_t chs[3], char str[8]) {
    sprintf(str, "%02x%02x%02x", chs[0], chs[1], chs[2]);
}

void Mbr_init(Mbr* mbr, const uint8_t* data) {
    const uint8_t* data_ = data;

    data_ = readTrait(mbr->boot_code, mbr->boot_code, &(data_));

    for (int i = 0; i < 4; ++i) {
        data_ = read8(&mbr->part_table[i].active, data_);
        data_ = readTrait(mbr->part_table[i].chs_start, mbr->part_table[i].chs_start, &data_);
        data_ = read8(&mbr->part_table[i].type, data_);
        data_ = readTrait(mbr->part_table[i].chs_end, mbr->part_table[i].chs_end, &data_);
        data_ = read32(&mbr->part_table[i].lba_start, data_);
        data_ = read32(&mbr->part_table[i].size, data_);
    }

    data_ = readTrait(mbr->last_signature, mbr->last_signature, &data_);
}

void print_mbr_info(Mbr* mbr) {
    for (int i = 0; i < 4; ++i) {
        struct Partition* p = &mbr->part_table[i];

        char chs_start_str[8], chs_end_str[8];
        bytes(p->chs_start, chs_start_str);
        bytes(p->chs_end, chs_end_str);

        printf("Partition %d\n", i);
        printf("  Active flag:         %02x\n", p->active);
        printf("  CHS Start:           C:%d, H:%d, S:%d (%s)\n", cylinder(p->chs_start), head(p->chs_start), sector(p->chs_start), chs_start_str);
        printf("  Partition Type:      %02x\n", p->type);
        printf("  CHS End:             C:%d, H:%d, S:%d (%s)\n", cylinder(p->chs_end), head(p->chs_end), sector(p->chs_end), chs_end_str);
        printf("  LBA Start:           %08x\n", p->lba_start);
        printf("  Number of Sectors:   %08x\n", p->size);
    }
}
