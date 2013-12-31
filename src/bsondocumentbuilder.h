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

#ifndef _BSON_BSONDOCUMENTBUILDER_H_
#define _BSON_BSONDOCUMENTBUILDER_H_

#include "bsontypes.h"
#include "bsonelement.h"
#include "bsondocument.h"
#include "oid.h"
#include "cpl_region.h"

/*
 * Generic struct for builder
 */
typedef struct bson_document_builder* bson_document_builder_ref;
struct bson_document_builder
{
    cpl_region_t    r;
};

/*
 * Default constructor
 */
static __inline__ bson_document_builder_ref bson_document_builder_create()
{
    bson_document_builder_ref bld = (bson_document_builder_ref)malloc(sizeof(struct bson_document_builder));
    if(bld)
    {
        cpl_region_init(&bld->r, 0);
    }
    return bld;
}

/*
 * Default destructor. Destroys the builder and the buffer.
 */
#define bson_document_builder_destroy(bld) cpl_region_deinit(&(bld)->r)

/*
 * Common destructor. Finalize and returns document. Frees builder.
 */
bson_document_ref bson_document_builder_finalize(bson_document_builder_ref __restrict bld);

/*
 * Append routines
 */
void bson_document_builder_append_el(bson_document_builder_ref __restrict bld,
                                     const bson_element_ref __restrict e);
void bson_document_builder_append_doc(bson_document_builder_ref __restrict bld,
                                      const char* __restrict k,
                                      const bson_document_ref __restrict doc);
// TODO: void bson_document_builder_append_arr
void bson_document_builder_append_b(bson_document_builder_ref __restrict bld,
                                    const char* __restrict k, _Bool b);
void bson_document_builder_append_i(bson_document_builder_ref __restrict bld,
                                    const char* __restrict k, int i);
void bson_document_builder_append_l(bson_document_builder_ref __restrict bld,
                                    const char* __restrict k, int64_t l);
void bson_document_builder_append_d(bson_document_builder_ref __restrict bld,
                                    const char* __restrict k, double d);
void bson_document_builder_append_oid(bson_document_builder_ref __restrict bld,
                                      const char* __restrict k,
                                      const bson_oid_ref __restrict oid);
void bson_document_builder_append_date(bson_document_builder_ref __restrict bld,
                                       const char* __restrict k, int64_t dt);
void bson_document_builder_append_str(bson_document_builder_ref __restrict bld,
                                      const char* __restrict k,
                                      const char* __restrict str);
void bson_document_builder_append_js(bson_document_builder_ref __restrict bld,
                                     const char* __restrict k,
                                     const char* __restrict js);
void bson_document_builder_append_null(bson_document_builder_ref __restrict bld,
                                       const char* __restrict k);

/* 
 * Append a regular expression value
 * @param regex the regular expression pattern
 * @param regex options such as "i" or "g"
 */
void bson_document_builder_append_regex(bson_document_builder_ref __restrict bld,
                                        const char* __restrict k,
                                        const char* __restrict regex,
                                        const char* __restrict flags);
void bson_document_builder_append_bin(bson_document_builder_ref __restrict bld,
                                      bson_subtype_t t, void* __restrict d,
                                      size_t sz);


#endif // _BSON_BSONDOCUMENTBUILDER_H_