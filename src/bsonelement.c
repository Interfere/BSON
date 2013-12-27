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

#include <string.h>

/*********************** BSON element private data ****************************/
struct bson_element
{
    union {
        bson_type_t type;       /* Type of the element */
        char        data[1];    /* Raw Data of an element */
    };
};

/********************** BSON element private interface ************************/
static size_t bson_element_fieldnamesize(bson_element_t e)
{
    return strlen(bson_element_fieldname(e)) + 1;
}

/********************** BSON element public interface *************************/
bson_type_t bson_element_type(bson_element_t e)
{
    return e->type;
}

const char* bson_element_fieldname(bson_element_t e)
{
    if(bson_element_type(e) == bson_type_eoo)
    {
        return 0;
    }
    
    return e->data + 1;
}

const char* bson_element_value(bson_element_t e)
{
    if(bson_element_type(e) == bson_type_eoo)
    {
        return 0;
    }
    
    return e->data + bson_element_fieldnamesize(e) + 1;
}
