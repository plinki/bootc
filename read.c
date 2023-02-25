#include "read.h"
#include <stdint.h>

const uint8_t* readTrait(const uint8_t* N, uint8_t* v, const uint8_t** data)
{
    memcpy(v, *data, sizeof(uint8_t) * (*N));
    *data += *N;
    return *data;
}

const uint8_t* read8(uint8_t* v, const uint8_t* data) {
    *v = data[0];
    return data + 1;
}

const uint8_t* read16(uint16_t* v, const uint8_t* data) {
    *v = ((uint16_t)data[1] << 8) | data[0];
    return data + 2;
}

const uint8_t* read24(uint32_t* v, const uint8_t* data) {
    *v = ((uint32_t)data[2] << 16) | ((uint32_t)data[1] << 8) | data[0];
    return data + 3;
}

const uint8_t* read32(uint32_t* v, const uint8_t* data) {
    *v = ((uint32_t)data[3] << 24) | ((uint32_t)data[2] << 16) | ((uint32_t)data[1] << 8) | data[0];
    return data + 4;
}

