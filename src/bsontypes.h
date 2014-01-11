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

#ifndef _BSON_BSONTYPES_H_
#define _BSON_BSONTYPES_H_

/**
 * See bsonspec.org
 */
enum bson_types
{
    /* smaller than other types */
    bson_type_minkey = -1,
    
    /* end of object */
    bson_type_eoo = 0,
    
    /* double precision floating point number */
    bson_type_float = 1,
    
    /* UTF-8 string */
    bson_type_string,
    
    /* an embedded document */
    bson_type_document,
    
    /* an embedded array */
    bson_type_array,
    
    /* binary data */
    bson_type_bindata,
    
    /* deprecated / undefined */
    bson_type_undefined,
    
    /* ObjectID */
    bson_type_oid,
    
    /* boolean type */
    bson_type_bool,
    
    /* Date */
    bson_type_date,
    
    /* Null */
    bson_type_null,
    
    /* regular expression type */
    bson_type_regex,
    
    /* deprecated */
    bson_type_dbpointer,
    
    /* deprecated / use CodeWScope */
    bson_type_code,
    
    /* deprecated / symbol */
    bson_type_symbol,
    
    /* javascript code that can execute on the database server, with SavedContext */
    bson_type_codewscope,
    
    /* 32 bit signed integer */
    bson_type_int,
    
    /* Internal (MongoDB) timestamp */
    bson_type_timestamp,
    
    /** 64 bit integer */
    bson_type_long,
    
    /* larger than all other types */
    bson_type_maxkey = 127
};
typedef char bson_type_t;

enum bson_subtype
{
    bson_subtype_generic = 0,
    bson_subtype_function,
    
    /* deprecated */
    bson_subtype_generic_old,
    /* deprecated */
    bson_subtype_uuid_old,
    
    bson_subtype_uuid,
    bson_subtype_md5,
    bson_subtype_custom = 128
};
typedef char bson_subtype_t;

#endif // _BSON_BSONTYPES_H_
