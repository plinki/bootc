#include <stdio.h>
#include <stdint.h>
#include "show.h"
#include "bootsector.h"

#define SECTOR_SIZE 512

int main(int argc, char* argv[]) {
    BootSector bs;
    bs.type = UNKNOWN;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("%s\n", "Usage: bootc [options] file");
            printf("%s\n", "Options:");
            printf("%s\n", "  -h, --help	   Print this message and exit");
            printf("%s\n", "  -t, --type TYPE  Set the boot sector type");
            printf("%s\n", "  -a, --asm		   Show assembly of the boot sector");
            return 0;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--type") == 0) {
            i++;
            if (i < argc) {
                char* type = argv[i];

                if (strcmp(type, "pbr") == 0) {
                    bs.type = PBR_FAT;
                }

                if (strcmp(type, "mbr") == 0) {
                    bs.type = MBR;
                }

            }
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--asm") == 0) {
            // a
            printf("%s\n", "a");
        } else {
            FILE* input = fopen(argv[i], "rb");
            if (!input) {
                printf("Error opening file %s\n", argv[1]);
                return 1;
            }

            uint8_t bs_buf[SECTOR_SIZE];
            size_t bytes_read = fread(bs_buf, 1, SECTOR_SIZE, input);
            if (bytes_read != SECTOR_SIZE) {
                printf("Size of input file must be 512 bytes. Bytes read: %zu\n", bytes_read);
                return 1;
            }

            do {
                if (bs.type == UNKNOWN) {
                    bs.type = infer(bs_buf);
                }
                if (bs.type == UNKNOWN) {
                    printf("%s\n", "Unknown boot sector type.");
                    return 1;
                }
            } while (0);

            fclose(input);
            BootSector_init(&bs, bs_buf);

            printf("%s%d\n", "Type: ", bs.type);
            printf("%02x%02x\n", bs.data[510], bs.data[511]);

            /*
            for (int i = 0; i < SECTOR_SIZE; i++) {
            	printf("%s%02x ", "0x", bs.data[i]);
            	if ((i+1) % 16 == 0) {
            		printf("\n");
               }
            }
            */
        }
    }

    return 0;
}

