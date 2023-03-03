#include <stdio.h>
#include <stdint.h>
#include <regex.h>
#include <stdlib.h>
#include <unistd.h>
#include "read.h"
#include "bootsector.h"
#include "tempfile.h"

#define MAX_LINE_LENGTH 1024

int determine_fat_type(PbrFat* fat);
const char dbwd_table[4] = {'b', 'w', 0, 'd'};

char *strndup(const char *s, size_t n) {
    char *p = strncpy(malloc(n + 1), s, n);
    if (p != NULL) {
        p[n] = '\0';
    }
    return p;
}

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

void print_info(PbrFat* fat) {
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

void print_asm(BootSector* bs) {
    const int fat_type = determine_fat_type(bs->Pbr_bs);

    unsigned int skipBytes = 0;

    struct instruction {
        uint32_t address;
        char* disassembly;
    };

    struct instruction_pair {
        uint32_t address;
        struct instruction inst;
    };

    if (bs->Pbr_bs->BS_jmpBoot[0] == 0xeb) {
        skipBytes = bs->Pbr_bs->BS_jmpBoot[1] - 1;
    } else if (bs->Pbr_bs->BS_jmpBoot[0] == 0xe9) {
        skipBytes = bs->Pbr_bs->BS_jmpBoot[1]
            | (unsigned int)(bs->Pbr_bs->BS_jmpBoot[2] << 8);
    }

    TemporaryFile* temp = create_tempfile();
    const char* path = tempfile_path(temp);
    size_t bs_size = strlen((const char *)bs->data);
    write(temp->fd, bs->data, bs_size);
    fsync(temp->fd);
    lseek(temp->fd, 0, SEEK_SET);
    // fclose(temp);

    const char* arg0 = "-b";
    const char* arg1 = "16";
    const char* arg2 = "-k";
    char arg3[50];
    sprintf(arg3, "3,%d", skipBytes);
    const char* arg4 = "-k";
    const char* arg5 = "510,2";
    const char* arg6 = path;

    char cmd[1024];
    sprintf(cmd, "ndisasm %s %s %s %s %s %s %s", arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    int status = system(cmd);
    if (status != 0) {
        printf("%d\n", status);
    } else {
        struct instruction_pair *instructions = NULL;
        int num_instructions = 0;
        int skip_index = -1;
        uint32_t zero_region_address = UINT32_MAX;

        const char* disasm_pattern = "^([0-9A-Fa-f]+)\\s+([0-9A-Fa-f]+)\\s+(.+)$";
        const char* skip_pattern = "^[0-9A-Fa-f]+\\s+skipping.*$";

        char line[MAX_LINE_LENGTH];
        ssize_t bytes_read = read(temp->fd, line, MAX_LINE_LENGTH);
        while ((bytes_read = read(temp->fd, line, MAX_LINE_LENGTH)) > 0) {
            regex_t regex_disasm, regex_skip;
            regmatch_t matches[4];
            int reti_disasm = regcomp(&regex_disasm, disasm_pattern, 0);
            int reti_skip = regcomp(&regex_skip, skip_pattern, 0);
            regfree(&regex_disasm);
            regfree(&regex_skip);
            if (reti_disasm != 0 || reti_skip != 0) {
                //err
            }

            if (regexec(&regex_disasm, line, 4, matches, 0) == 0) {
                uint32_t address = (uint32_t)strtoul(&line[matches[1].rm_so], NULL, 16);

                if (strncmp(&line[matches[2].rm_so], "0000", 4) == 0) {
                    if (zero_region_address == UINT32_MAX) {
                        zero_region_address = address;
                    }
                } else {
                    zero_region_address = UINT32_MAX;
                    char* disassembly = strndup(&line[matches[3].rm_so], matches[3].rm_eo - matches[3].rm_so);
                    struct instruction inst = { address, disassembly };
                    struct instruction_pair pair = { address, inst };
                    num_instructions++;
                    instructions = realloc(instructions, num_instructions * sizeof(struct instruction_pair));
                    instructions[num_instructions-1] = pair;
                }
            } else if (regexec(&regex_skip, line, 0, NULL, 0) == 0) {
                if (skip_index == -1) {
                    skip_index = num_instructions;
                }
            }
        }

        for (int i = 0; i < num_instructions; ++i) {
            if (i == skip_index) {
                uint32_t current = (i > 0) ? instructions[i - 1].address : 0;
                const uint32_t lim = instructions[i].address;
                FILE* temp_file = fdopen(temp->fd, "r");

                current = db_str(temp_file, current, lim, bs->Pbr_bs->BS_OEMName, 8);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_BytsPerSec);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_SecPerClus);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_RsvdSecCnt);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_NumFATs);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_RootEntCnt);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_TotSec16);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_Media);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_FATSz16);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_SecPerTrk);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_NumHeads);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_HiddSec);
                current = dbwd(temp_file, current, lim, bs->Pbr_bs->BPB_TotSec32);

                if (fat_type == 12 || fat_type == 16) {
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat12_16.BS_DrvNum);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat12_16.BS_Reserved1);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat12_16.BS_BootSig);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat12_16.BS_VolID);
                    current = db_str(temp_file, current, lim, bs->Pbr_bs->fat12_16.BS_VolLab, 11);
                    current = db_str(temp_file, current, lim, bs->Pbr_bs->fat12_16.BS_FilSysType, 8);
                } else {
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BPB_FATSz32);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BPB_ExtFlags);
                    current = dbwd(temp_file, current, lim, *bs->Pbr_bs->fat32.BPB_FSVer);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BPB_RootClus);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BPB_FSInfo);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BPB_BkBootSec);
                    current = dbwd(temp_file, current, lim, *bs->Pbr_bs->fat32.BPB_Reserved);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BS_DrvNum);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BS_Reserved1);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BS_BootSig);
                    current = dbwd(temp_file, current, lim, bs->Pbr_bs->fat32.BS_VolID);
                    current = db_str(temp_file, current, lim, bs->Pbr_bs->fat32.BS_VolLab, 11);
                    current = db_str(temp_file, current, lim, bs->Pbr_bs->fat32.BS_FilSysType, 8);
                }

                fclose(temp_file);

                if (current < lim) {
                    printf("    times 0x%02x - ($ - $$) db 0\n", lim);
                }
                
                if (i == 0) {
                    if (bs->Pbr_bs->BS_jmpBoot[0] == 0xeb) {
                        printf("    jmp short entry\n");
                    } else {
                        printf("    jmp near entry\n");
                    }
                } else {
                    printf("    %08x %s\n", instructions[i].inst.address, instructions[i].inst.disassembly); 
                }
            }

            if (zero_region_address != UINT32_MAX) {
                printf("    times 0x1fe - ($ - $$) db 0\n");
            }

            printf("    db 0x55, 0xaa\n");
        }
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

