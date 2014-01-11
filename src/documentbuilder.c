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

#include "documentbuilder.h"

extern inline bson_document_builder_ref bson_document_builder_create();
extern inline bson_document_ref bson_document_builder_finalize(bson_document_builder_ref __restrict bld);

extern inline void bson_document_builder_append_b(bson_document_builder_ref __restrict bld,
                                                  const char* __restrict k, char b);
extern inline void bson_document_builder_append_i(bson_document_builder_ref __restrict bld,
                                                  const char* __restrict k, int32_t i);
extern inline void bson_document_builder_append_oid(bson_document_builder_ref __restrict bld,
                                             const char* __restrict k,
                                             const bson_oid_ref __restrict oid);
extern inline void bson_document_builder_append_str(bson_document_builder_ref __restrict bld,
                                                    const char* __restrict k,
                                                    const char* __restrict str);