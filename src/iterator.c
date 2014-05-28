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

#include "iterator.h"


extern inline int bson_iterator_end(bson_iterator_ref __restrict i);
extern inline bson_element_ref bson_iterator_init(bson_iterator_ref __restrict i, bson_document_ref doc);

void bson_iterator_next_el(bson_iterator_ref __restrict i, bson_element_ref* e)
{
    if(i->next_off == 0)
        return ;
    
    i->curr_off = i->next_off;
    
    *e = (bson_element_ref)(i->d->data + i->curr_off);
    
    if(bson_element_type(*e) == bson_type_eoo)
    {
        i->next_off = 0;
    }
    else
    {
        i->next_off = i->curr_off + bson_element_size(*e);
    }
}

bson_element_ref bson_iterator_next(bson_iterator_ref __restrict i)
{
    if(i->next_off == 0)
        return 0;
    
    i->curr_off = i->next_off;
    
    bson_element_ref el = bson_element_create_with_data(i->d->data + i->curr_off);
    
    if(bson_element_type(el) == bson_type_eoo)
    {
        i->next_off = 0;
    }
    else
    {
        i->next_off = i->curr_off + bson_element_size(el);
    }
    
    return el;
}