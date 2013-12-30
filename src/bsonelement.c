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

#include "bsonelement.h"

/****************************** Private Impl **********************************/
struct bson_element_internal
{
    const char*     data;
    size_t          fieldNameSize;
    size_t          totalSize;
};
typedef struct bson_element_internal* bson_element_internal_ref;

/****************************** Public Impl ***********************************/
bson_element_ref bson_element_create()
{
    static char z = 0;
    bson_element_internal_ref __restrict e = (bson_element_internal_ref)malloc(sizeof(struct bson_element_internal));
    e->data = &z;
    e->fieldNameSize = 0;
    e->totalSize = 1;
    return (bson_element_ref)e;
}

bson_element_ref bson_element_create_with_data(const char *d)
{
    struct bson_element_internal* __restrict e = (struct bson_element_internal*)malloc(sizeof(struct bson_element_internal));
    e->data = d;
    if(bson_element_type((bson_element_ref)e) == bson_type_eoo)
    {
        e->fieldNameSize = 0;
        e->totalSize = 1;
    }
    else
    {
        e->fieldNameSize = -1;
        e->totalSize = -1;
    }
    return (bson_element_ref)e;
}

size_t bson_element_fieldnamesize(bson_element_ref e)
{
    struct bson_element_internal* __restrict internal = (struct bson_element_internal* __restrict)e;
    
    if(internal->fieldNameSize == -1)
    {
        internal->fieldNameSize = strlen(bson_element_fieldname(e)) + 1;
    }
    
    return internal->fieldNameSize;
}
