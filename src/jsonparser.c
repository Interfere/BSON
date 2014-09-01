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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "documentbuilder.h"
#include <cpl/cpl_array.h>

#define HexDigit(c)				((c) <= '9' ? (c) - '0' : (toupper((c)) & 7) + 9)

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
    JT_KEY,
    JT_STRING,
    JT_INT,
    JT_FLOAT,
    JT_FALSE,
    JT_TRUE,
    JT_NULL
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
    };
};

struct json_parser
{
    void        *data;
    const char  *cur;
    const char  *end;
    struct json_token last_token;
    struct json_token key_token;
};

static void json_parser_start_object(struct json_parser* parser)
{
    printf("START_OBJECT\n");
    
    cpl_array_ref a = (cpl_array_ref)parser->data;
    if(!a)
    {
        a = cpl_array_create(sizeof(bson_document_builder_ref), 64);
        parser->data = a;
    }
    
    bson_document_builder_ref parent = 0;
    if(a->count)
    {
        parent = cpl_array_back(a, bson_document_builder_ref);
    }
    
    if(parent)
    {
        char* buffer = 0;
        size_t len;
        if(parser->last_token.type == JT_KEY)
        {
            len = parser->last_token.length + 2;
            buffer = (char *)malloc(len);
            *buffer = bson_type_document;
            memcpy(buffer + 1, parser->last_token.start, parser->last_token.length);
            buffer[len-1] = '\0';
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

static void json_parser_finish_object(struct json_parser* parser)
{
    printf("END_OBJECT\n");
    cpl_array_ref a = (cpl_array_ref)parser->data;
    
    bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
    cpl_array_pop_back(a);
    
    (void)bson_document_builder_finalize(b);
}

static void json_parser_start_array(struct json_parser* parser)
{
    printf("START_ARRAY\n");
    cpl_array_ref a = (cpl_array_ref)parser->data;
    if(!a)
    {
        a = cpl_array_create(sizeof(bson_document_builder_ref), 64);
        parser->data = a;
    }
    
    bson_document_builder_ref parent = 0;
    if(a->count)
    {
        parent = cpl_array_back(a, bson_document_builder_ref);
    }
    
    if(parent)
    {
        char* buffer = 0;
        size_t len;
        if(parser->last_token.type == JT_KEY)
        {
            len = parser->last_token.length + 2;
            buffer = (char *)malloc(len);
            *buffer = bson_type_array;
            memcpy(buffer + 1, parser->last_token.start, parser->last_token.length);
            buffer[len-1] = '\0';
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

static void json_parser_finish_array(struct json_parser* parser)
{
    printf("END_ARRAY\n");
    cpl_array_ref a = (cpl_array_ref)parser->data;
    
    bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
    cpl_array_pop_back(a);
    
    (void)bson_document_builder_finalize(b);
}

static void json_parser_colon(struct json_parser* parser)
{
    printf("COLON\n");
    parser->key_token = parser->last_token;
    parser->key_token.type = JT_KEY;
}

static void json_parser_comma(struct json_parser* parser)
{
    printf("COMMA\n");
    cpl_array_ref a = (cpl_array_ref)parser->data;
    bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
    if(parser->key_token.type == JT_UNDEF)
    {
        bson_array_builder_append_str(b, parser->last_token.start, parser->last_token.length);
    }
    else
    {
        bson_document_builder_append_str(b, parser->key_token.start, parser->last_token.start, parser->last_token.length);
    }
}

static void json_parser_string(struct json_parser* parser)
{
    printf("STRING \"%.*s\"\n", (int)parser->last_token.length, parser->last_token.start);
}

static void json_parser_num(struct json_parser* parser)
{
    printf("NUM %.*s\n", (int)parser->last_token.length, parser->last_token.start);
    if(parser->key_token.type == JT_UNDEF)
    {
        cpl_array_ref a = (cpl_array_ref)parser->data;
        bson_document_builder_ref b = cpl_array_back(a, bson_document_builder_ref);
        
    }
}

static void json_parser_term(struct json_parser* parser)
{
    printf("TERM %.*s\n", (int)parser->last_token.length, parser->last_token.start);
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

int json_parser_parse(const char *json, size_t nlength)
{
    struct json_parser parser = {
        .data = 0,
        .cur = json,
        .end = json + nlength
    };
    
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
                json_parser_string(&parser);
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
                json_parser_num(&parser);
                break;
                
            case 'n':
                if(json_parse_null(&parser))
                {
                    // error
                    return 1;
                }
                json_parser_term(&parser);
                break;
                
            case 't':
                if(json_parse_true(&parser))
                {
                    // error
                    return 1;
                }
                json_parser_term(&parser);
                break;
                
            case 'f':
                if(json_parse_false(&parser))
                {
                    // error
                    return 1;
                }
                json_parser_term(&parser);
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

