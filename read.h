#ifndef READ_H
#define READ_H

#include <stddef.h>
#include <stdint.h>


const uint8_t* read8(uint8_t* v, const uint8_t* data);
const uint8_t* read16(uint16_t* v, const uint8_t* data);
const uint8_t* read24(uint32_t* v, const uint8_t* data);
const uint8_t* read32(uint32_t* v, const uint8_t* data);


#define read(N, v, data) \
    do { \
        memcpy((v), (data), (N)); \
        (data) += (N); \
    } while (0)

#endif /* READ_H */

