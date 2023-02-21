#ifndef SHOW_H_
#define SHOW_H_

#include <string.h>
#include <stdint.h>
#include <stddef.h>

typedef struct FixedLength {
    char *s_;
    size_t length_;
} FixedLength;

FixedLength fixlen(char *s, size_t length);

typedef struct HexBytes {
    uint8_t *s_;
    size_t length_;
} HexBytes;

HexBytes bytes(const uint8_t *v, size_t length);

void print_hex_bytes(const HexBytes* b, size_t n);

void print_uint8_t(uint8_t v);

#endif /* SHOW_H_ */

