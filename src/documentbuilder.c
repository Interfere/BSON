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

extern inline bson_document_builder_ref bson_document_builder_create_with_parent(bson_document_builder_ref parent);
extern inline bson_document_ref bson_document_builder_finalize(bson_document_builder_ref __restrict bld);

extern inline void bson_document_builder_append_el(bson_document_builder_ref __restrict bld,
                                                   const bson_element_ref __restrict e);
extern inline void bson_document_builder_append_doc(bson_document_builder_ref __restrict bld,
                                                    const char* __restrict k,
                                                    const bson_document_ref __restrict doc);
extern inline void bson_document_builder_append_arr(bson_document_builder_ref __restrict bld,
                                                    const char* __restrict k,
                                                    const bson_array_ref __restrict arr);
extern inline void bson_document_builder_append_b(bson_document_builder_ref __restrict bld,
                                                  const char* __restrict k, char b);
extern inline void bson_document_builder_append_i(bson_document_builder_ref __restrict bld,
                                                  const char* __restrict k, int32_t i);
extern inline void bson_document_builder_append_l(bson_document_builder_ref __restrict bld,
                                                  const char* __restrict k, int64_t l);
extern inline void bson_document_builder_append_d(bson_document_builder_ref __restrict bld,
                                                  const char* __restrict k, double d);
extern inline void bson_document_builder_append_oid(bson_document_builder_ref __restrict bld,
                                                    const char* __restrict k,
                                                    const bson_oid_ref __restrict oid);
extern inline void bson_document_builder_append_date(bson_document_builder_ref __restrict bld,
                                                     const char* __restrict k, int64_t dt);
extern inline void bson_document_builder_append_str(bson_document_builder_ref __restrict bld,
                                                    const char* __restrict k,
                                                    const char* __restrict str);
extern inline void bson_document_builder_append_js(bson_document_builder_ref __restrict bld,
                                                   const char* __restrict k,
                                                   const char* __restrict js);
extern inline void bson_document_builder_append_null(bson_document_builder_ref __restrict bld,
                                                     const char* __restrict k);
extern inline void bson_document_builder_append_regex(bson_document_builder_ref __restrict bld,
                                                      const char* __restrict k,
                                                      const char* __restrict regex,
                                                      const char* __restrict flags);
extern inline void bson_document_builder_append_bin(bson_document_builder_ref __restrict bld,
                                                    const char* __restrict k,
                                                    bson_subtype_t t, void* __restrict d,
                                                    int32_t sz);
