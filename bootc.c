#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bootsector.h"

#define SECTOR_SIZE 512

int argument_used(char* cmpr, char* arg, char* argument) {
    return (strcmp(cmpr, arg) == 0 || strcmp(cmpr, argument) == 0);
}

int main(int argc, char* argv[]) {
    enum Type bs_type = UNKNOWN;
    int show_asm = 0;

    for (int i = 1; i < argc; i++) {
        if (argument_used(argv[i], "-h", "--help")) {
            printf("%s\n", "Usage: bootc [options] file");
            printf("%s\n", "Options:");
            printf("%s\n", "  -h, --help	   Print this message and exit");
            printf("%s\n", "  -t, --type TYPE  Set the boot sector type");
            printf("%s\n", "  -a, --asm		   Show assembly of the boot sector");
            return 0;
        } else if (argument_used(argv[i], "-t", "--t")) {
            i++;
            char* type = argv[i];

            if (strcmp(type, "pbr") == 0) {
                bs_type = PBR_FAT;
            }

            if (strcmp(type, "mbr") == 0) {
                bs_type = MBR;
            }
        } else if (argument_used(argv[i], "-a", "--asm")) {
            show_asm = 1;
        } else {
            FILE* input = fopen(argv[i], "rb");
            if (!input) {
                printf("Error opening file %s\n", argv[i]);
                return 1;
            }

            uint8_t bs_buf[SECTOR_SIZE];
            size_t bytes_read = fread(bs_buf, 1, SECTOR_SIZE, input);
            if (bytes_read != SECTOR_SIZE) {
                printf("Size of input file must be 512 bytes. Bytes read: %zu\n", bytes_read);
                return 1;
            }

            fclose(input);
            BootSector* bs = make_bs(bs_buf, infer(bs_buf));
            BootSector_init(bs, bs_buf);

            bs->type = (bs_type == UNKNOWN) ? infer(bs_buf) : bs_type;

            if (show_asm) {
               print_asm(bs);
            } else {
                print_info(bs->Pbr_bs);
            }
        }

    }
    return 0;
}

