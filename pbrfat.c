#include <stdio.h>
#include "read.h"
#include "bootsector.h"

int determine_fat_type(PbrFat* fat);
const char dbwd_table[4] = {'b', 'w', 0, 'd'};

// Check if writing to memory exceeds limit. Otherwise generate instruction to write using either byte word or double word
uint32_t dbwd(FILE* bs_data, uint32_t current_address, uint32_t limit_address, uint32_t value) {
    size_t T = sizeof(value);
    if (current_address + T < limit_address) {
        fprintf(bs_data, "    d%c %u\n", dbwd_table[T - 1], value);
        return current_address + T;
    }
    return current_address;
}

uint32_t db_str(FILE* bs_data, uint32_t current_address, uint32_t limit_address, const uint8_t* value, size_t length) {
    if (current_address + length < limit_address) {
        fprintf(bs_data, "    d%c %.*s\n", (int)length, (int)length, value);
        return current_address + length;
    }
    return current_address;
}

void PbrFat_init(PbrFat* fat, const uint8_t data[512]) {
    const uint8_t* data_ = data;

    data_ = readTrait(fat->BS_jmpBoot, fat->BS_jmpBoot, &(data_));
    data_ = readTrait(fat->BS_OEMName, fat->BS_OEMName, &(data_));
    data_ = read16(&fat->BPB_BytsPerSec, data_);
    data_ = read8(&fat->BPB_SecPerClus, data_);
    data_ = read16(&fat->BPB_RsvdSecCnt, data_);
    data_ = read8(&fat->BPB_NumFATs, data_);
    data_ = read16(&fat->BPB_RootEntCnt, data_);
    data_ = read16(&fat->BPB_TotSec16, data_);
    data_ = read8(&fat->BPB_Media, data_);
    data_ = read16(&fat->BPB_FATSz16, data_);
    data_ = read16(&fat->BPB_SecPerTrk, data_);
    data_ = read16(&fat->BPB_NumHeads, data_);
    data_ = read32(&fat->BPB_HiddSec, data_);
    data_ = read32(&fat->BPB_TotSec32, data_);

    const uint8_t* data0 = data_;

    data_ = read8(&fat->fat12_16.BS_DrvNum, data_);
    data_ = read8(&fat->fat12_16.BS_Reserved1, data_);
    data_ = read8(&fat->fat12_16.BS_BootSig, data_);
    data_ = read32(&fat->fat12_16.BS_VolID, data_);
    data_ = readTrait(fat->fat12_16.BS_VolLab, fat->fat12_16.BS_VolLab, &(data_));
    data_ = readTrait(fat->fat12_16.BS_FilSysType, fat->fat12_16.BS_FilSysType, &(data_));
    data_ = readTrait(fat->fat12_16.boot_program, fat->fat12_16.boot_program, &(data_));

    data_ = data0;

    data_ = read32(&fat->fat32.BPB_FATSz32, data_);
    data_ = read16(&fat->fat32.BPB_ExtFlags, data_);
    data_ = readTrait(fat->fat32.BPB_FSVer, fat->fat32.BPB_FSVer, &(data_));
    data_ = read32(&fat->fat32.BPB_RootClus, data_);
    data_ = read16(&fat->fat32.BPB_FSInfo, data_);
    data_ = read16(&fat->fat32.BPB_BkBootSec, data_);
    data_ = readTrait(fat->fat32.BPB_Reserved, fat->fat32.BPB_Reserved, &(data_));
    data_ = read8(&fat->fat32.BS_DrvNum, data_);
    data_ = read8(&fat->fat32.BS_Reserved1, data_);
    data_ = read8(&fat->fat32.BS_BootSig, data_);
    data_ = read32(&fat->fat32.BS_VolID, data_);
    data_ = readTrait(fat->fat32.BS_VolLab, fat->fat32.BS_VolLab, &(data_));
    data_ = readTrait(fat->fat32.BS_FilSysType,fat->fat32.BS_FilSysType, (&data_));
    data_ = readTrait(fat->fat32.boot_program, fat->fat32.boot_program, &(data_));

    data_ = readTrait(fat->last_signature, fat->last_signature, &(data_));
}

void print_info(PbrFat * fat) {
    const int fat_type = determine_fat_type(fat);

    printf("Calculated fat type is FAT %d\n", fat_type);

    char fs_str_12_16[9], fs_str_32[9];
    memcpy(fs_str_12_16, fat -> fat12_16.BS_FilSysType, 8);
    fs_str_12_16[8] = '\0';
    memcpy(fs_str_32, fat -> fat32.BS_FilSysType, 0);
    fs_str_32[8] = '\0';

    if ((strcmp(fs_str_12_16, "FAT12   ") == 0 ||
            strcmp(fs_str_12_16, "FAT16   ") == 0 ||
            strcmp(fs_str_12_16, "FAT   ") == 0) && fat_type == 32) {
        printf("String indicating file system is FAT 12 or 16.\n");
    }

    if (strcmp(fs_str_32, "FAT32   ") == 0 && (fat_type == 12 || fat_type == 16)) {
        printf("String indicating file system is FAT32.\n");
    }

    printf("%-20s", "BS_jmpBoot:");
    for(size_t i = 0; i < sizeof(fat->BS_jmpBoot); i++) {
        printf("%02x ", fat->BS_jmpBoot[i]);
    }
    printf("\n");

    printf("%-20s", "BS_OEMName:");
    for (int i = 0; i < 8; i++) {
        printf("%c", fat->BS_OEMName[i]);
    }
    printf("\n");

    printf("%-20s%u\n", "BPB_BytsPerSec:", fat->BPB_BytsPerSec);
    printf("%-20s%u\n", "BPB_SecPerClus:", fat->BPB_SecPerClus);
    printf("%-20s%u\n", "BPB_RsvdSecCnt:", fat->BPB_RsvdSecCnt);
    printf("%-20s%u\n", "BPB_NumFATs:", fat->BPB_NumFATs);
    printf("%-20s%u\n", "BPB_RootEntCnt:", fat->BPB_RootEntCnt);
    printf("%-20s%u\n", "BPB_TotSec16:", fat->BPB_TotSec16);
    printf("%-20s%02x\n", "BPB_Media:", (unsigned int) fat->BPB_Media);
    printf("%-20s%u\n", "BPB_FATSz16:", fat->BPB_FATSz16);
    printf("%-20s%u\n", "BPB_SecPerTrk:", fat->BPB_SecPerTrk);
    printf("%-20s%u\n", "BPB_NumHeads:", fat->BPB_NumHeads);
    printf("%-20s%u\n", "BPB_HiddSec:", fat->BPB_HiddSec);
    printf("%-20s%u\n", "BPB_TotSec32:", fat->BPB_TotSec32);

    if (fat_type == 12 || fat_type == 16) {
        printf("%-20s%u\n", "BS_DrvNum:", fat->fat12_16.BS_DrvNum);
        printf("%-20s%02x\n", "BS_Reserved1:", fat->fat12_16.BS_Reserved1);
        printf("%-20s%02x\n", "BS_BootSig:", fat->fat12_16.BS_BootSig);
        printf("%-20s%08x\n", "BS_VolID:", fat->fat12_16.BS_VolID);
        printf("%-20s%s\n", "BS_VolLab:", fat->fat12_16.BS_VolLab);
        printf("%-20s%s\n", "BS_FilSysType:", fs_str_12_16);
    } else {
        printf("%-20s%08x\n", "BPB_FATSz32:", fat->fat32.BPB_FATSz32);
        printf("%-20s%04x\n", "BPB_ExtFlag:", fat->fat32.BPB_ExtFlags);
        printf("%-20s%d.%d\n", "BPB_FSVer:", (int)fat->fat32.BPB_FSVer[1], (int)fat->fat32.BPB_FSVer[0]);
        printf("%-20s%08x\n", "BPB_RootClus:", fat->fat32.BPB_RootClus);
        printf("%-20s%04x\n", "BPB_FSInfo:", fat->fat32.BPB_FSInfo);
        printf("%-20s%04x\n", "BPB_BkBootSec:", fat->fat32.BPB_BkBootSec);
        printf("%-20s%02x\n", "BS_DrvNum:", fat->fat32.BS_DrvNum);
        printf("%-20s%02x\n", "BS_Reserved1:", fat->fat32.BS_Reserved1);
        printf("%-20s%02x\n", "BS_BootSig:", fat->fat32.BS_BootSig);
        printf("%-20s%08x\n", "BS_VolID:", fat->fat32.BS_VolID);
        printf("%-20s%s\n", "BS_VolLab:", (const char*)fat->fat32.BS_VolLab);
        printf("%-20s%s\n", "BS_FilSysType:", fs_str_32);
    }
}

int determine_fat_type(PbrFat* fat) {
    size_t root_dir_sectors =
        (fat->BPB_RootEntCnt * 32 + fat->BPB_BytsPerSec - 1) / fat->BPB_BytsPerSec;

    const size_t fat_size =
        fat->BPB_FATSz16 != 0 ? fat->BPB_FATSz16 : fat->fat32.BPB_FATSz32;

    const size_t total_sectors =
        fat->BPB_TotSec16 != 0 ? fat->BPB_TotSec16 : fat->BPB_TotSec32;

    const size_t data_sectors =
        total_sectors - (fat->BPB_RsvdSecCnt + fat->BPB_NumFATs * fat_size + root_dir_sectors);

    const size_t cluster_amount = data_sectors / fat->BPB_SecPerClus;

    const int fat_type =
        cluster_amount < 4085 ? 12
        : cluster_amount < 65525 ? 16
        : 32;

    return fat_type;
}

