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

/*
 * C Primitives Library. Array (aka vector) implementation.
 */

#ifndef _CPL_ARRAY_H_
#define _CPL_ARRAY_H_

#include <stdlib.h>

#define _CPL_DEFAULT_ARRAY_SIZE     64

struct cpl_array
{
    char*       data;
};
typedef struct cpl_array* cpl_array_ref;

/*
 * Creates empty array.
 */
static __inline__ cpl_array_ref cpl_array_create()
{
    return
}

/*
 * Destroys an array.
 */
stratic __inline__ cpl_array_destroy(cpl_array_ref __restrict a)
{
    free(a);
}

#endif // _CPL_ARRAY_H_
