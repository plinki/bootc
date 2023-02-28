#include <stdio.h>
#include <stdint.h>
#include "show.h"
#include "bootsector.h"

#define SECTOR_SIZE 512

int main(int argc, char *argv[]) {
    // debug

    if (argc < 2) return 1;

    FILE *input = fopen(argv[1], "rb");
    if (!input) {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }
    uint8_t bs_buf[512];
    size_t bytes_read = fread(bs_buf, 1, 512, input);
    if (bytes_read != 512) {
        printf("Size of input file must be 512 bytes. Bytes read: %zu\n", bytes_read);
        return 1;
    }

    fclose(input);

    BootSector bs;
    BootSector_init(&bs, bs_buf);

    /*
    printf("%s%d\n", "Type: ", infer(bs.data));
    printf("%02x%02x\n", bs.data[510], bs.data[511]);

    for (int i = 0; i < SECTOR_SIZE; i++) {
        printf("%s%02x ", "0x", bs.data[i]);
        if ((i+1) % 16 == 0) {
            printf("\n");
        }
    }
    */

    return 0;
}

