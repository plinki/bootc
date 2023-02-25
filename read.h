#ifndef READ_H
#define READ_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>


const uint8_t* read8(uint8_t* v, const uint8_t* data);
const uint8_t* read16(uint16_t* v, const uint8_t* data);
const uint8_t* read24(uint32_t* v, const uint8_t* data);
const uint8_t* read32(uint32_t* v, const uint8_t* data);


const uint8_t* readTrait(const uint8_t* N, uint8_t* v, const uint8_t** data);

#endif /* READ_H */

