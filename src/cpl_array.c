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

cpl_array_ref cpl_array_create(size_t sz, size_t nreserv)
{
    cpl_array_ref a = (cpl_array_ref)malloc(sizeof(struct cpl_array));
    if(a)
    {
        a->szelem = sz;
        a->nreserv = nreserv;
        a->count = 0;
        a->data = malloc(sz * nreserv);
        if(!a->data)
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
    size_t new_count = a->count + sz / a->szelem;
    size_t nreserv = a->nreserv;
    while(new_count >= nreserv)
    {
        nreserv *= 2;
    }
    
    /* Try to reallocate the buffer */
    void* ptr = realloc(a->data, nreserv * a->szelem);
    if(!ptr)
    {
        return _CPL_NOMEM;
    }
    a->data = ptr;
    a->nreserv = nreserv;
    memcpy(cpl_array_get_p(a, cpl_array_count(a) - 1), p, sz);
    a->count += new_count;
    
    return _CPL_OK;
}
