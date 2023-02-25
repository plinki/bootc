#include "read.h"
#include "bootsector.h"

void PbrFat_init(PbrFat* fat, const uint8_t data[512]) {
    const uint8_t* data_ = data;

    //data_ = read(fat->BS_jmpBoot, &(fat->BS_jmpBoot) ,data_);
    //data_ = read(fat->BS_OEMName, &(fat->BS_OEMName), data_);
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
    //data_ = read(fat->fat12_16.BS_VolLab, &(fat->fat12_16.BS_VolLab), data_);
    //data_ = read(fat->fat12_16.BS_FilSysType, &(fat->fat12_16.BS_FilSysType), data_);
    //data_ = read(fat->fat12_16.boot_program, &(fat->fat12_16.boot_program), data_);

    data_ = data0;

    data_ = read32(&fat->fat32.BPB_FATSz32, data_);
    data_ = read16(&fat->fat32.BPB_ExtFlags, data_);
    //data_ = read(fat->fat32.BPB_FSVer, &(fat32.BPB_FSVer), data_);
    data_ = read32(&fat->fat32.BPB_RootClus, data_);
    data_ = read16(&fat->fat32.BPB_FSInfo, data_);
    data_ = read16(&fat->fat32.BPB_BkBootSec, data_);
    //data_ = read(fat->fat32.BPB_Reserved, &(fat->fat32.BPB_Reserved), data_);
    data_ = read8(&fat->fat32.BS_DrvNum, data_);
    data_ = read8(&fat->fat32.BS_Reserved1, data_);
    data_ = read8(&fat->fat32.BS_BootSig, data_);
    data_ = read32(&fat->fat32.BS_VolID, data_);
    //data_ = read(fat->fat32.BS_VolLab, &(fat->fat32.BS_VolLab), data_);
    //data_ = read(fat->fat32.BS_FilSysType,&(fat->fat32.BS_FilSysType), data_);
    //data_ = read(fat->fat32.boot_program, &(fat->fat32.boot_program), data_);

    //data_ = read(fat->last_signature, &(fat->last_signature), data_);
}

