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

#ifndef _BSON_BSONDOCUMENT_H_
#define _BSON_BSONDOCUMENT_H_

#include <stdint.h>
#include <stdlib.h>

/**
 * Generic type for bson document
 */
typedef struct bson_document* bson_document_ref;
struct bson_document
{
    const char      *data;  /* Raw Data of the document */
};

/**
 * Creates empty document
 */
static __inline__ bson_document_ref bson_document_create()
{
    static char p[] = { /* size */5, 0, 0, 0, /* eoo */0 };
    bson_document_ref __restrict e = (bson_document_ref)malloc(sizeof(struct bson_document));
    e->data = p;
    return e;
}

/**
 * Creates document with given data
 */
static __inline__ bson_document_ref bson_document_create_with_data(const char *d)
{
    bson_document_ref __restrict e = (bson_document_ref)malloc(sizeof(struct bson_document));
    e->data = d;
    return e;
}

/**
 * Destroy document
 */
static __inline__ void bson_document_destroy(bson_document_ref __restrict doc)
{
    free(doc);
}

/**
 * Get size of the document
 */
static __inline__ int32_t bson_document_size(bson_document_ref __restrict doc)
{
    return *(int32_t *)doc->data;
}


#endif // _BSON_BSONDOCUMENT_H_
