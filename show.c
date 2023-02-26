#include <stdio.h>
#include "show.h"

void fixlen(ostream_t* os, const FixedLength* f) {
    int i;

    fprintf(os, "%s", f->s);
    for (i = strlen(f->s); i < f->length; ++i) {
        fputc(' ', os);
    }
}

void print_hex_bytes(ostream_t* os, const hexbytes_t* b) {
    char pat[4];
    int i;

    sprintf(pat, "%%02x");
    fprintf(os, pat, (int)b->s[0]);
    for (i = 1; i < b->length; ++i) {
        fprintf(os, " ");
        fprintf(os, pat, (int)b->s[i]);
    }
}

void print_uint8_t(ostream_t* os, uint8_t v) {
    fprintf(os, "%d", v);
}

