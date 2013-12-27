/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Alexey Komnin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _BSON_OID_H_
#define _BSON_OID_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/**
 * ObjectID constant size in bytes
 */
static const int bson_oid_size = 12;

/**
 * ObjectID's inc field size in bytes
 */
static const int bson_oid_inc_size = 3;

/**
 * Generic ObjectID type
 */
#pragma pack(1)
typedef struct bson_oid* bson_oid_ref;
struct bson_oid
{
    union {
        struct {
            int32_t x;
            int32_t y;
            int32_t z;
        };
        struct {
            int64_t a;
            int32_t b;
        };
        struct {
            uint8_t time[4];
            struct machine_and_pid {
                uint8_t     machine_number[3];
                uint16_t    pid;
            } machine_and_pid;
            uint8_t inc[bson_oid_inc_size];
        };
        unsigned char data[bson_oid_size];
    };
};
#pragma pack()

/**
 * Default constructor for bson_oid object
 */
bson_oid_ref bson_oid_create();

/**
 * Create ObjectID with 24 char hex representation
 */
bson_oid_ref bson_oid_create_with_string(const char *);

/**
 * Create ObjectID with byte array
 */
bson_oid_ref bson_oid_create_with_bytes(const uint8_t arr[bson_oid_size]);

/**
 * Destruct ObjectID
 */
static inline void bson_oid_destroy(bson_oid_ref __restrict oid)
{
    free(oid);
}

/**
 * Clear the ObjectID
 */
static inline void bson_oid_clear(bson_oid_ref __restrict oid)
{
    oid->a = 0;
    oid->b = 0;
}

/**
 * Compare two ObjectIDs
 */
static inline int bson_oid_compare(const bson_oid_ref __restrict oidl,
                            const bson_oid_ref __restrict oidr)
{
    return memcmp(oidl->data, oidr->data, bson_oid_size);
}

/**
 * Create string representation of ObjectID
 */
char* bson_oid_string_create(const bson_oid_ref oid);


#endif // _BSON_OID_H_
