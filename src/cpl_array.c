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

#include "cpl_array.h"
#include "cpl_error.h"

cpl_array_ref cpl_array_create(size_t sz, size_t nreserv)
{
    cpl_array_ref a = (cpl_array_ref)malloc(sizeof(struct cpl_array));
    if(a)
    {
        a->szelem = sz;
        a->count = 0;
        int res = cpl_region_init(&a->region, sz * nreserv);
        if(res != _CPL_OK)
        {
            free(a);
            a = 0;
        }
    }
    return a;
}


int cpl_array_push_back_p(cpl_array_ref a, void* p, size_t sz)
{
    /* Size of buffer should be at least size of an element */
    if(sz < a->szelem) return _CPL_INVALID_ARG;
    
    /* get new count of elements in array */
    size_t count = sz / a->szelem;
    
    cpl_region_append_data(&a->region, p, count * a->szelem);
    a->count += count;
    
    return _CPL_OK;
}