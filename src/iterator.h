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

#ifndef _BSON_ITERATOR_H_
#define _BSON_ITERATOR_H_

#include "element.h"
#include "document.h"

typedef struct bson_iterator bson_iterator_t;
typedef struct bson_iterator *bson_iterator_ref;
struct bson_iterator
{
    bson_document_ref d;
    size_t     curr_off;
    size_t     next_off;
};

bson_element_ref bson_iterator_next(bson_iterator_ref __restrict i);
void bson_iterator_next_el(bson_iterator_ref __restrict i, bson_element_ref e);

inline int bson_iterator_end(bson_iterator_ref __restrict i)
{
    return i->next_off == 0;
}

inline bson_element_ref bson_iterator_init(bson_iterator_ref __restrict i, bson_document_ref doc)
{
    i->d = doc;
    i->curr_off = 0;
    i->next_off = 4;
    return bson_iterator_next(i);
}

#endif // _BSON_ITERATOR_H_
