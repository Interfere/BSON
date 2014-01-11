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

#include "element.h"

#include <assert.h>
#include "oid.h"

/****************************** Public Impl ***********************************/
extern inline size_t bson_element_key_size(bson_element_ref __restrict e);
extern inline bson_element_ref bson_element_create();
extern inline const char* bson_element_value(bson_element_ref __restrict e);
extern inline const char* bson_element_fieldname(bson_element_ref __restrict e);

bson_element_ref bson_element_create_with_data(const char *d)
{
    struct bson_element* __restrict e = (struct bson_element*)malloc(sizeof(struct bson_element));
    if(e)
    {
        e->data = d;
        if(bson_element_type(e) == bson_type_eoo)
        {
            e->key_size = 0;
            e->size = 1;
        }
        else
        {
            e->key_size = -1;
            e->size = -1;
        }
    }
    return e;
}

size_t bson_element_size(bson_element_ref __restrict e)
{
    if(e->size == -1)
    {
        switch (bson_element_type(e)) {
            case bson_type_date:
            case bson_type_long:
            case bson_type_float:
            case bson_type_timestamp:
                e->size = 1 + bson_element_key_size(e) + 8;
                break;
                
            case bson_type_code:
            case bson_type_symbol:
            case bson_type_string:
            case bson_type_array:
            case bson_type_document:
            {
                int32_t len = *(int32_t*)(bson_element_fieldname(e) + bson_element_key_size(e));
                e->size = 1 + bson_element_key_size(e) + sizeof(len) + len;
                break;
            }
                
            case bson_type_bindata:
            {
                int32_t len = *(int32_t*)(bson_element_fieldname(e) + bson_element_key_size(e));
                e->size = 1 + bson_element_key_size(e) + sizeof(len) + 1 + len;
                break;
            }
            
            case bson_type_oid:
                e->size = 1 + bson_element_key_size(e) + bson_oid_size;
                break;
                
            case bson_type_bool:
                e->size = 1 + bson_element_key_size(e) + 1;
                break;
                
            case bson_type_regex:
            {
                const char* pattern = bson_element_fieldname(e) + bson_element_key_size(e);
                size_t pattern_len = strlen(pattern) + 1;
                const char* opts = pattern + pattern_len;
                size_t opts_len = strlen(opts) + 1;
                e->size = 1 + bson_element_key_size(e) + pattern_len + opts_len;
                break;
            }
                
            case bson_type_codewscope:
            {
                const char* code = bson_element_fieldname(e) + bson_element_key_size(e);
                int32_t code_len = *(int32_t*)code;
                const char* scope = code + sizeof(code_len) + code_len;
                int32_t scope_len = *(int32_t*)scope;
                e->size = 1 + bson_element_key_size(e) + sizeof(code_len) + code_len + sizeof(scope_len) + scope_len;
            }
                
            case bson_type_int:
                e->size = 1 + bson_element_key_size(e) + 4;
                break;
                
            case bson_type_undefined:
            case bson_type_minkey:
            case bson_type_maxkey:
                e->size = 1 + bson_element_key_size(e);
                break;
                
            case bson_type_eoo:
            default:
                assert(0);
                break;
        }
    }
    return e->size;
}

