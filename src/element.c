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

size_t bson_element_value_size(bson_element_ref __restrict e)
{
    size_t size = 0;
    switch (bson_element_type(e)) {
        case bson_type_date:
        case bson_type_long:
        case bson_type_float:
        case bson_type_timestamp:
            size = 8;
            break;
            
        case bson_type_code:
        case bson_type_symbol:
        case bson_type_string:
        {
            int32_t len = *(int32_t*)bson_element_value(e);
            size = sizeof(len) + len;
            break;
        }
        case bson_type_array:
        case bson_type_document:
        {
            size = *(int32_t*)bson_element_value(e);
            break;
        }
            
        case bson_type_bindata:
        {
            int32_t len = *(int32_t*)bson_element_value(e);
            size = sizeof(len) + 1 + len;
            break;
        }
            
        case bson_type_oid:
            size = bson_oid_size;
            break;
            
        case bson_type_bool:
            size = 1;
            break;
            
        case bson_type_regex:
        {
            const char* pattern = bson_element_value(e);
            size_t pattern_len = strlen(pattern) + 1;
            const char* opts = pattern + pattern_len;
            size_t opts_len = strlen(opts) + 1;
            size = pattern_len + opts_len;
            break;
        }
            
        case bson_type_codewscope:
        {
            const char* code = bson_element_value(e);
            int32_t code_len = *(int32_t*)code;
            const char* scope = code + sizeof(code_len) + code_len;
            int32_t scope_len = *(int32_t*)scope;
            size = sizeof(code_len) + code_len + sizeof(scope_len) + scope_len;
            break;
        }
            
        case bson_type_int:
            size = 4;
            break;
            
        case bson_type_undefined:
        case bson_type_minkey:
        case bson_type_maxkey:
            break;
            
        case bson_type_eoo:
        default:
            assert(0);
            break;
    }
    
    return size;
}

