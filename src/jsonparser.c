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

#include "jsonparser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// For debug purpose only
#include <stdarg.h>

#include "documentbuilder.h"
#include <cpl/cpl_array.h>

enum json_parser_lexems
{
    JL_LBRACE = '{',
    JL_RBRACE = '}',
    JL_LBRACKET = '[',
    JL_RBRACKET = ']',
    JL_COMMA = ',',
    JL_COLON = ':',
    JL_DQUOT = '"',
    JL_BACKSLASH = '\\',
    JL_FRACDOT = '.',
    JL_EXP = 'e'
};

enum json_parser_tokens
{
    JT_UNDEF = 0,
    JT_STRING,
    JT_INT,
    JT_FLOAT,
    JT_NULL,
    JT_OID,
    JT_KEY,
    JT_FALSE,
    JT_TRUE
};

struct json_token
{
    enum json_parser_tokens type;
    const char *start;
    size_t length;
    union
    {
        int has_escape;
        long ivalue;
        double fvalue;
        bson_oid_t oid;
    };
};

struct json_parser
{
    struct json_parser_callbacks callbacks;
    void        *data;
    const char  *cur;
    const char  *end;
    struct json_token last_token;
    struct json_token key_token;
};

static void json_parser_colon(struct json_parser* parser)
{
    parser->key_token = parser->last_token;
    parser->key_token.type = JT_KEY;
}

static void jsonProductPair(struct json_parser* parser)
{
    if(!parser->callbacks.xProductPair) return;
    
    char* key = strndup(parser->key_token.start, parser->key_token.length);
    
    switch (parser->last_token.type) {
        case JT_STRING:
        {
            char* val = strndup(parser->last_token.start, parser->last_token.length);
            
            parser->callbacks.xProductPair(parser->data, key, bson_type_string, val);
            
            free(val);
            break;
        }
            
        case JT_FLOAT:
            parser->callbacks.xProductPair(parser->data, key, bson_type_float, parser->last_token.fvalue);
            break;
            
        case JT_INT:
            parser->callbacks.xProductPair(parser->data, key, bson_type_int, parser->last_token.ivalue);
            break;
            
        case JT_TRUE:
            parser->callbacks.xProductPair(parser->data, key, bson_type_bool, 1);
            break;
            
        case JT_FALSE:
            parser->callbacks.xProductPair(parser->data, key, bson_type_bool, 0);
            break;
            
        case JT_NULL:
            parser->callbacks.xProductPair(parser->data, key, bson_type_null);
            break;
            
        case JT_OID:
            parser->callbacks.xProductPair(parser->data, key, bson_type_oid, &parser->last_token.oid);
            break;
            
        case JT_KEY:
        default:
            assert(0);
            break;
    }
    
    free(key);
}

static void jsonProductValue(struct json_parser* parser)
{
    if(!parser->callbacks.xProductVal) return;
    
    switch (parser->last_token.type) {
        case JT_STRING:
        {
            char* val = strndup(parser->last_token.start, parser->last_token.length);
            
            parser->callbacks.xProductVal(parser->data, bson_type_string, val);
            
            free(val);
            break;
        }
            
        case JT_FLOAT:
            parser->callbacks.xProductVal(parser->data, bson_type_float, parser->last_token.fvalue);
            break;
            
        case JT_INT:
            parser->callbacks.xProductVal(parser->data, bson_type_int, parser->last_token.ivalue);
            break;
            
        case JT_TRUE:
            parser->callbacks.xProductVal(parser->data, bson_type_bool, 1);
            break;
            
        case JT_FALSE:
            parser->callbacks.xProductVal(parser->data, bson_type_bool, 0);
            break;
            
        case JT_NULL:
            parser->callbacks.xProductVal(parser->data, bson_type_null);
            break;
            
        case JT_OID:
            parser->callbacks.xProductVal(parser->data, bson_type_oid, &parser->last_token.oid);
            break;
            
        case JT_KEY:
        default:
            assert(0);
            break;
    }
}

static void json_parser_comma(struct json_parser* parser)
{
    if(parser->last_token.type != JT_UNDEF)
    {
        if(parser->key_token.type == JT_UNDEF)
        {
            jsonProductValue(parser);
        }
        else
        {
            jsonProductPair(parser);
        }
        
        parser->key_token.type = JT_UNDEF;
        parser->last_token.type = JT_UNDEF;
    }
}

static void json_parser_start_object(struct json_parser* parser)
{
    if(parser->callbacks.xStartObject)
    {
        if(parser->key_token.type == JT_KEY)
        {
            parser->callbacks.xStartObject(parser->data, parser->key_token.start, parser->key_token.length);
        }
        else
        {
            parser->callbacks.xStartObject(parser->data, 0, 0);
        }
    }
    
    parser->key_token.type = JT_UNDEF;
    parser->last_token.type = JT_UNDEF;
}

static void json_parser_finish_object(struct json_parser* parser)
{
    if(parser->last_token.type != JT_UNDEF)
    {
        jsonProductPair(parser);
    }
    
    if(parser->callbacks.xEndObject)
    {
        parser->callbacks.xEndObject(parser->data);
    }
    
    parser->last_token.type = JT_UNDEF;
}

static void json_parser_start_array(struct json_parser* parser)
{
    if(parser->callbacks.xStartArray)
    {
        if(parser->key_token.type == JT_KEY)
        {
            parser->callbacks.xStartArray(parser->data, parser->key_token.start, parser->key_token.length);
        }
        else
        {
            parser->callbacks.xStartArray(parser->data, 0, 0);
        }
    }
    
    parser->key_token.type = JT_UNDEF;
    parser->last_token.type = JT_UNDEF;
}

static void json_parser_finish_array(struct json_parser* parser)
{
    if(parser->last_token.type != JT_UNDEF)
    {
        jsonProductValue(parser);
    }
 
    if(parser->callbacks.xEndArray)
    {
        parser->callbacks.xEndArray(parser->data);
    }
    
    parser->last_token.type = JT_UNDEF;
}

static int json_parse_string(struct json_parser* parser)
{
    parser->last_token.type = JT_STRING;
    parser->last_token.start = ++parser->cur;
    parser->last_token.length = 0;
    parser->last_token.has_escape = 0;
    
    for (; parser->cur != parser->end; ++parser->cur)
    {
        char c = *parser->cur;
        
        if(c == JL_DQUOT)
        {
            parser->last_token.length = parser->cur - parser->last_token.start;
            return 0;
        }
        if(c < 0x20)
        {
            return 1; // TODO: error control-character
        }
        
        if(c == JL_BACKSLASH)
        {
            parser->last_token.has_escape = 1;
            parser->cur++;
            switch (*parser->cur) {
                case '\"': case '/': case 'b': case 't':
                case '\\': case 'f': case 'r': case 'n':
                    break;
                    
                case 'u':
                    parser->cur += 3;
                    break;
                    
                default:
                    return 1; // TODO: return error invalid token
            }
        }
    }
    
    return 1; // TODO: return error partial json
}

static int json_parse_null(struct json_parser* parser)
{
    parser->last_token.type = JT_NULL;
    parser->last_token.start = parser->cur;
    parser->last_token.length = 4;
    if(tolower(*++parser->cur) == 'u' &&
       tolower(*++parser->cur) == 'l' &&
       tolower(*++parser->cur) == 'l')
    {
        return 0;
    }
    return 1;
}

static int json_parse_true(struct json_parser* parser)
{
    parser->last_token.type = JT_TRUE;
    parser->last_token.start = parser->cur;
    parser->last_token.length = 4;
    if(tolower(*++parser->cur) == 'r' &&
       tolower(*++parser->cur) == 'u' &&
       tolower(*++parser->cur) == 'e')
    {
        return 0;
    }
    return 1;
}

static int json_parse_false(struct json_parser* parser)
{
    parser->last_token.type = JT_FALSE;
    parser->last_token.start = parser->cur;
    parser->last_token.length = 5;
    if(tolower(*++parser->cur) == 'a' &&
       tolower(*++parser->cur) == 'l' &&
       tolower(*++parser->cur) == 's' &&
       tolower(*++parser->cur) == 'e')
    {
        return 0;
    }
    return 1;
}

static int json_parse_oid(struct json_parser* parser)
{
    static const char oid_tok[] = "ObjectId(\"";
    if(memcmp(parser->cur, oid_tok, sizeof(oid_tok) - 1))
    {
        return 1;
    }
    parser->cur += sizeof(oid_tok) - 2;
    
    if(json_parse_string(parser))
    {
        return 1;
    }
    
    if(*++parser->cur != ')')
    {
        return 1;
    }
    
    if(parser->last_token.length < 24)
    {
        return 1;
    }
    
    parser->last_token.type = JT_OID;
    bson_oid_init_with_str(&parser->last_token.oid, parser->last_token.start);
    return 0;
}

static int json_parse_num(struct json_parser* parser)
{
    parser->last_token.type = JT_INT;
    parser->last_token.start = parser->cur;
    int has_exp = 0;
    for (; parser->cur != parser->end; ++parser->cur)
    {
        switch (*parser->cur) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                break;
                
            case 'e': case 'E':
                if(!has_exp)
                {
                    has_exp = 1;
                    parser->last_token.type = JT_FLOAT;
                    char c = *(parser->cur+1);
                    if(isdigit(c))
                    {
                        break;
                    }
                    if(c=='+' || c=='-')
                    {
                        parser->cur++;
                        break;
                    }
                }
                return 1;
                
            case '.':
                if(parser->last_token.type == JT_INT)
                {
                    parser->last_token.type = JT_FLOAT;
                    break;
                }
                return 1;
                
            case '-':
                if(parser->cur != parser->last_token.start)
                {
                    return 1;
                }
                break;
                
            case ' ': case '\t': case '\n': case ',': case '}': case ']':
                goto Lexit;
                
            default: return 1;
        }
    }
    
Lexit:
    if(parser->last_token.type == JT_INT)
    {
        parser->last_token.ivalue = atoi(parser->last_token.start);
    }
    else
    {
        parser->last_token.fvalue = atof(parser->last_token.start);
    }
    parser->last_token.length = parser->cur - parser->last_token.start;
    parser->cur--;
    
    return 0;
}

int json_parser_parse(const char *json, size_t nlength, struct json_parser_callbacks* callbacks, void* data)
{
    struct json_parser parser = {
        .callbacks = {0},
        .data = data,
        .cur = json,
        .end = json + nlength,
        .last_token = {
            .type = JT_UNDEF
        }
    };
    
    if(callbacks)
    {
        parser.callbacks = *callbacks;
    }
    
    for(;parser.cur != parser.end; ++parser.cur)
    {
        char ch = *parser.cur;
        switch (ch) {
            case JL_LBRACE:
                json_parser_start_object(&parser);
                break;
                
            case JL_RBRACE:
                json_parser_finish_object(&parser);
                break;
                
            case JL_LBRACKET:
                json_parser_start_array(&parser);
                break;
                
            case JL_RBRACKET:
                json_parser_finish_array(&parser);
                break;
                
            case JL_DQUOT:
                if(json_parse_string(&parser) != 0)
                {
                    // error
                    return 1;
                }
                break;
                
            case JL_COLON:
                json_parser_colon(&parser);
                break;
                
            case JL_COMMA:
                json_parser_comma(&parser);
                break;
                
            case '-': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                if(json_parse_num(&parser))
                {
                    return 1;
                }
                break;
                
            case 'n':
                if(json_parse_null(&parser))
                {
                    // error
                    return 1;
                }
                break;
                
            case 't':
                if(json_parse_true(&parser))
                {
                    // error
                    return 1;
                }
                break;
                
            case 'f':
                if(json_parse_false(&parser))
                {
                    // error
                    return 1;
                }
                break;
                
            case 'O':
                if(json_parse_oid(&parser))
                {
                    return 1;
                }
                break;
            
            case ' ': case '\t': case '\n': case '\r':
                // skip whitespace
                break;
                
            default:
                return 1;
        }
    }
    
    return 0;
}

// JSON 2 BSON SECTION
struct _helper
{
    cpl_array_t a;
    bson_document_ref d;
};

static void xProductPair(void *data, const char *key, bson_type_t type, ...)
{
    struct _helper* h = (struct _helper *)data;
    cpl_array_ref a = &h->a;
    bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
    
    va_list v;
    va_start(v, type);
    
    switch (type) {
        case bson_type_null:
            bson_document_builder_append_null(b, key);
            break;
            
        case bson_type_bool:
            bson_document_builder_append_b(b, key, va_arg(v, int));
            break;
            
        case bson_type_int:
            bson_document_builder_append_i(b, key, va_arg(v, long));
            break;
            
        case bson_type_float:
            bson_document_builder_append_d(b, key, va_arg(v, double));
            break;
            
        case bson_type_string:
            bson_document_builder_append_str(b, key, va_arg(v, char *));
            break;
            
        case bson_type_oid:
            bson_document_builder_append_oid(b, key, va_arg(v, bson_oid_ref));
            break;
            
        default:
            assert(0);
            break;
    }
    
    va_end(v);
}

static void xProductVal(void *data, bson_type_t type, ...)
{
    struct _helper* h = (struct _helper *)data;
    cpl_array_ref a = &h->a;
    bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
    
    va_list v;
    va_start(v, type);
    
    switch (type) {
        case bson_type_null:
            bson_array_builder_append_null(b);
            break;
            
        case bson_type_bool:
            bson_array_builder_append_b(b, va_arg(v, int));
            break;
            
        case bson_type_int:
            bson_array_builder_append_i(b, va_arg(v, long));
            break;
            
        case bson_type_float:
            bson_array_builder_append_d(b, va_arg(v, double));
            break;
            
        case bson_type_string:
            bson_array_builder_append_str(b, va_arg(v, char *));
            break;
            
        case bson_type_oid:
            bson_array_builder_append_oid(b, va_arg(v, bson_oid_ref));
            break;
            
        default:
            assert(0);
            break;
    }
    
    va_end(v);
}

static void xStartObject(void *data, const char *key, size_t nkey)
{
    struct _helper* h = (struct _helper *)data;
    cpl_array_ref a = &h->a;
    
    bson_document_builder_ref parent = 0;
    if(cpl_array_count(a))
    {
        parent = cpl_array_back(a, bson_document_builder_ref);
    }
    
    if(parent)
    {
        char* buffer = 0;
        size_t len;
        if(key)
        {
            len = nkey + 2;
            buffer = (char *)malloc(len);
            *buffer = bson_type_document;
            strncpy(buffer + 1, key, nkey);
            buffer[len - 1] = '\0';
        }
        else
        {
            len = 16 + 2;
            buffer = (char *)malloc(len);
            *buffer = bson_type_document;
            len = sprintf(buffer+1, "%lu", parent->index++) + 2;
        }
        
        cpl_region_append_data(&parent->r, buffer, len);
        
        free(buffer);
    }
    
    bson_document_builder_ref b = bson_document_builder_create_with_parent(parent);
    cpl_array_push_back(a, b);
}

static void xStartArray(void *data, const char *key, size_t nkey)
{
    struct _helper* h = (struct _helper *)data;
    cpl_array_ref a = &h->a;
    
    bson_document_builder_ref parent = 0;
    if(cpl_array_count(a))
    {
        parent = cpl_array_back(a, bson_document_builder_ref);
    }
    
    if(parent)
    {
        char* buffer = 0;
        size_t len;
        if(key)
        {
            len = nkey + 2;
            buffer = (char *)malloc(len);
            *buffer = bson_type_array;
            strncpy(buffer + 1, key, nkey);
            buffer[len - 1] = '\0';
        }
        else
        {
            len = 16 + 2;
            buffer = (char *)malloc(len);
            *buffer = bson_type_array;
            len = sprintf(buffer+1, "%lu", parent->index++) + 2;
        }
        
        cpl_region_append_data(&parent->r, buffer, len);
        
        free(buffer);
    }
    
    bson_document_builder_ref b = bson_document_builder_create_with_parent(parent);
    cpl_array_push_back(a, b);
}

static void xEndObject(void *data)
{
    struct _helper* h = (struct _helper *)data;
    cpl_array_ref a = &h->a;
    
    bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
    cpl_array_pop_back(a);
    
    bson_document_ref d = bson_document_builder_finalize(b);
    
    if(cpl_array_count(a) == 0)
    {
        h->d = d;
    }
}

bson_document_ref json2bson(const char *json, size_t nlength)
{
    struct _helper h;
    cpl_array_init(&h.a, sizeof(bson_document_builder_ref), 16);
    struct json_parser_callbacks c =
    {
        .xProductPair = xProductPair,
        .xProductVal = xProductVal,
        .xStartObject = xStartObject,
        .xEndObject = xEndObject,
        .xStartArray = xStartArray,
        .xEndArray = xEndObject
    };
    
    int rc = json_parser_parse(json, nlength, &c, &h);
    if(rc)
    {
        // TODO: report error
        return 0;
    }
    
    return h.d;
}

