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

#ifndef _BSON_DOCUMENT_H_
#define _BSON_DOCUMENT_H_

#include <stdint.h>
#include <stdlib.h>
#include <bson/element.h>

/**
 * Generic type for bson document
 */
typedef struct bson_document* bson_document_ref;
struct bson_document
{
    const char data[5];     /* Raw Data of the document */
};

/**
 * Creates empty document
 */
static inline bson_document_ref bson_document_create()
{
    static const char p[] = { /* size */5, 0, 0, 0, /* eoo */0 };
    bson_document_ref __restrict e = (bson_document_ref)&p;
    return e;
}

/**
 * Destroy document
 */
static inline void bson_document_destroy(bson_document_ref __restrict doc)
{
    if(doc->data != bson_document_create()->data)
    {
        free(doc);
    }
}

/**
 * Get size of the document
 */
#define bson_document_size(doc)             (*(int32_t *)doc->data)

/**
 * Get first element of the document
 */
#define bson_document_get_first(doc)        bson_element_create_with_data(doc->data + 4);

/**
 * Array is a special type of document, with indexes as keys
 */
#define bson_array_ref                      bson_document_ref
#define bson_array_create()                 bson_document_create()
#define bson_array_create_with_data(d)      bson_document_create_with_data(d)
#define bson_array_destroy(a)               bson_document_destroy(a)
#define bson_array_get_first(a)             bson_document_get_first(a)

#endif // _BSON_DOCUMENT_H_
