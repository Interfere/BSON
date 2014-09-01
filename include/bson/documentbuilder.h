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

#ifndef _BSON_DOCUMENTBUILDER_H_
#define _BSON_DOCUMENTBUILDER_H_

#include <limits.h>
#include <stdio.h>

#include <bson/bsontypes.h>
#include <bson/element.h>
#include <bson/document.h>
#include <bson/oid.h>
#include <cpl/cpl_region.h>

/*
 * Generic struct for builder
 */
typedef struct bson_document_builder* bson_document_builder_ref;
struct bson_document_builder
{
    cpl_region_t    r;
    size_t          off;
    size_t          index;      /* An index to build array instead of document */
};

/*
 * Default constructor
 */
inline bson_document_builder_ref bson_document_builder_create_with_parent(bson_document_builder_ref parent)
{
    bson_document_builder_ref bld = (bson_document_builder_ref)malloc(sizeof(struct bson_document_builder));
    if(bld)
    {
        
        if(parent)
        {
            bld->r = parent->r;
            bld->r.offset += sizeof(int32_t);
            bld->off = parent->r.offset;
        }
        else
        {
            cpl_region_init(cpl_allocator_get_default(), &bld->r, 0);
            bld->r.offset = sizeof(int32_t);
            bld->off = 0;
        }
        bld->index = 0;
    }
    return bld;
}

static inline bson_document_builder_ref bson_document_builder_create()
{
    return bson_document_builder_create_with_parent(0);
}

/*
 * Default destructor. Destroys the builder and the buffer.
 */
#define bson_document_builder_destroy(bld) cpl_region_deinit(&(bld)->r);free(bld)

/*
 * Common destructor. Finalize and returns document. Frees builder.
 */
inline bson_document_ref bson_document_builder_finalize(bson_document_builder_ref __restrict bld)
{
    bson_type_t type = bson_type_eoo;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    int32_t* count = bld->r.data + bld->off;
    *count = (int32_t)(bld->r.offset - bld->off);
    
    bson_document_ref doc = (bson_document_ref)count;
    free(bld);
    return doc;
}

/*
 * Append routines
 */
inline void bson_document_builder_append_el(bson_document_builder_ref __restrict bld,
                                            const bson_element_ref __restrict e)
{
    cpl_region_append_data(&bld->r, e->data, bson_element_size(e));
}

inline void bson_document_builder_append_doc(bson_document_builder_ref __restrict bld,
                                             const char* __restrict k,
                                             const bson_document_ref __restrict doc)
{
    bson_type_t type = bson_type_document;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k) + 1);
    cpl_region_append_data(&bld->r, doc->data, bson_document_size(doc));
}

inline void bson_document_builder_append_arr(bson_document_builder_ref __restrict bld,
                                             const char* __restrict k,
                                             const bson_array_ref __restrict arr)
{
    bson_type_t type = bson_type_array;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k) + 1);
    cpl_region_append_data(&bld->r, arr->data, bson_document_size(arr));
}

inline void bson_document_builder_append_b(bson_document_builder_ref __restrict bld,
                                           const char* __restrict k, char b)
{
    bson_type_t type = bson_type_bool;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, &b, sizeof(b));
}

inline void bson_document_builder_append_i(bson_document_builder_ref __restrict bld,
                                           const char* __restrict k, int32_t i)
{
    bson_type_t type = bson_type_int;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, &i, sizeof(i));
}

inline void bson_document_builder_append_l(bson_document_builder_ref __restrict bld,
                                           const char* __restrict k, int64_t l)
{
    bson_type_t type = bson_type_long;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, &l, sizeof(l));
}

inline void bson_document_builder_append_d(bson_document_builder_ref __restrict bld,
                                           const char* __restrict k, double d)
{
    bson_type_t type = bson_type_float;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, &d, sizeof(d));
}

inline void bson_document_builder_append_oid(bson_document_builder_ref __restrict bld,
                                             const char* __restrict k,
                                             const bson_oid_ref __restrict oid)
{
    bson_type_t type = bson_type_oid;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, oid->data, sizeof(oid->data));
}

inline void bson_document_builder_append_date(bson_document_builder_ref __restrict bld,
                                              const char* __restrict k, int64_t dt)
{
    bson_type_t type = bson_type_date;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, &dt, sizeof(dt));
}

inline void bson_document_builder_append_str(bson_document_builder_ref __restrict bld,
                                             const char* __restrict k,
                                             const char* __restrict str,
                                             int32_t size)
{
    bson_type_t type = bson_type_string;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    
    size += 1;
    cpl_region_append_data(&bld->r, &size, sizeof(size));
    cpl_region_append_data(&bld->r, str, size);
}

inline void bson_document_builder_append_js(bson_document_builder_ref __restrict bld,
                                            const char* __restrict k,
                                            const char* __restrict js)
{
    bson_type_t type = bson_type_code;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    
    int32_t size = (int32_t)strlen(js) + 1;
    cpl_region_append_data(&bld->r, &size, sizeof(size));
    cpl_region_append_data(&bld->r, js, size);
}

inline void bson_document_builder_append_null(bson_document_builder_ref __restrict bld,
                                              const char* __restrict k)
{
    bson_type_t type = bson_type_null;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
}

/*
 * Append a regular expression value
 * @param regex the regular expression pattern
 * @param regex options such as "i" or "g"
 */
inline void bson_document_builder_append_regex(bson_document_builder_ref __restrict bld,
                                               const char* __restrict k,
                                               const char* __restrict regex,
                                               const char* __restrict flags)
{
    bson_type_t type = bson_type_regex;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, regex, strlen(regex)+1);
    if(!flags)
    {
        flags = "";
    }
    cpl_region_append_data(&bld->r, flags, strlen(flags) + 1);
}

inline void bson_document_builder_append_bin(bson_document_builder_ref __restrict bld,
                                             const char* __restrict k,
                                             bson_subtype_t t, void* __restrict d,
                                             int32_t sz)
{
    bson_type_t type = bson_type_bindata;
    cpl_region_append_data(&bld->r, &type, sizeof(type));
    cpl_region_append_data(&bld->r, k, strlen(k)+1);
    cpl_region_append_data(&bld->r, &sz, sizeof(sz));
    cpl_region_append_data(&bld->r, &t, sizeof(t));
    cpl_region_append_data(&bld->r, d, sz);
}

/**
 * BSON Array builder
 */
#define bson_array_builder_ref              bson_document_builder_ref
#define bson_array_builder_create()         bson_document_builder_create()
#define bson_array_builder_destroy(bld)     bson_document_builder_destroy(bld)
#define bson_array_builder_finalize(bld)    bson_document_builder_finalize(bld)

static inline void bson_array_builder_append_doc(bson_array_builder_ref __restrict bld,
                                                 const bson_document_ref __restrict doc)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_doc(bld, k, doc);
}

static inline void bson_array_builder_append_arr(bson_array_builder_ref __restrict bld,
                                                 const bson_array_ref __restrict arr)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_arr(bld, k, arr);
}

static inline void bson_array_builder_append_b(bson_array_builder_ref __restrict bld,
                                               char b)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_b(bld, k, b);
}

static inline void bson_array_builder_append_i(bson_array_builder_ref __restrict bld,
                                               int32_t i)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_i(bld, k, i);
}

static inline void bson_array_builder_append_l(bson_array_builder_ref __restrict bld,
                                               int64_t l)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_l(bld, k, l);
}

static inline void bson_array_builder_append_d(bson_array_builder_ref __restrict bld,
                                               double d)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_d(bld, k, d);
}

static inline void bson_array_builder_append_oid(bson_document_builder_ref __restrict bld,
                                                 const bson_oid_ref __restrict oid)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_oid(bld, k, oid);
}

static inline void bson_array_builder_append_date(bson_array_builder_ref __restrict bld,
                                                  int64_t dt)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_date(bld, k, dt);
}

static inline void bson_array_builder_append_str(bson_array_builder_ref __restrict bld,
                                                 const char* __restrict str, int32_t size)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_str(bld, k, str, size);
}

static inline void bson_array_builder_append_js(bson_array_builder_ref __restrict bld,
                                            const char* __restrict js)
{
    char k[16];
    sprintf(k, "%lu", bld->index);
    bson_document_builder_append_js(bld, k, js);
}

static inline void bson_array_builder_append_null(bson_array_builder_ref __restrict bld)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_null(bld, k);
}

static inline void bson_array_builder_append_regex(bson_array_builder_ref __restrict bld,
                                                   const char* __restrict regex,
                                                   const char* __restrict flags)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_regex(bld, k, regex, flags);
}

static inline void bson_array_builder_append_bin(bson_array_builder_ref __restrict bld,
                                             bson_subtype_t t, void* __restrict d,
                                             int32_t sz)
{
    char k[16];
    sprintf(k, "%lu", bld->index++);
    bson_document_builder_append_bin(bld, k, t, d, sz);
}

#endif // _BSON_DOCUMENTBUILDER_H_
