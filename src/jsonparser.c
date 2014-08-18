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
    };
};

struct json_parser
{
    const char *cur;
    const char *end;
    struct json_token* last_token;
};

static void json_parser_start_object(struct json_token *tok)
{
    printf("JL_LBRACE");
    if(tok)
    {
        printf(": %.*s", (int)tok->length, tok->start);
    }
    printf("\n");
}

static void json_parser_finish_object()
{
    printf("JL_RBRACE\n");
}

static int json_parse_string(struct json_parser* parser)
{
    parser->last_token->type = JT_STRING;
    parser->last_token->start = ++parser->cur;
    parser->last_token->length = 0;
    parser->last_token->has_escape = 0;
    
    for (; parser->cur != parser->end; ++parser->cur)
    {
        char c = *parser->cur;
        
        if(c == JL_DQUOT)
        {
            parser->last_token->length = parser->cur - parser->last_token->start;
            return 0;
        }
        if(c < 0x20)
        {
            return 1; // TODO: error control-character
        }
        
        if(c == JL_BACKSLASH)
        {
            parser->last_token->has_escape = 1;
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
    parser->last_token->type = JT_NULL;
    parser->last_token->start = parser->cur;
    parser->last_token->length = 4;
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
    parser->last_token->type = JT_TRUE;
    parser->last_token->start = parser->cur;
    parser->last_token->length = 4;
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
    parser->last_token->type = JT_FALSE;
    parser->last_token->start = parser->cur;
    parser->last_token->length = 5;
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
    
}

static void json_parser_product_pair(struct json_parser *parser,
                                struct json_token* key,
                                struct json_token* val)
{
    printf("PRODUCT: %.*s : %.*s\n", (int)key->length, key->start, (int)val->length, val->start);
}

int json_parser_parse(const char *json, size_t nlength)
{
    struct json_parser parser = {
        .cur = json,
        .end = json + nlength,
        .last_token = 0
    };
    enum { KEY_TOKEN = 0, VAL_TOKEN = 1 };
    struct json_token tokens[2];
    
    for(;parser.cur != parser.end; ++parser.cur)
    {
        char ch = *parser.cur;
        switch (ch) {
            case JL_LBRACE:
                if(parser.last_token)
                {
                    json_parser_start_object(&tokens[KEY_TOKEN]);
                }
                else
                {
                    json_parser_start_object(0);
                }
                parser.last_token = &tokens[KEY_TOKEN];
                break;
                
            case JL_RBRACE:
                json_parser_product_pair(&parser, &tokens[KEY_TOKEN], &tokens[VAL_TOKEN]);
                json_parser_finish_object();
                parser.last_token = 0;
                break;
                
            case JL_DQUOT:
                if(json_parse_string(&parser) != 0)
                {
                    // error
                    return 1;
                }
                break;
                
            case JL_COLON:
                parser.last_token = &tokens[VAL_TOKEN];
                break;
                
            case JL_COMMA:
                if(parser.last_token)
                {
                    json_parser_product_pair(&parser, &tokens[KEY_TOKEN], &tokens[VAL_TOKEN]);
                }
                parser.last_token = &tokens[KEY_TOKEN];
                break;
                
            case '-': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                json_parse_num(&parser);
                break;
                
            case 'n':
                if(json_parse_null(&parser) != 0)
                {
                    // error
                    return 1;
                }
                printf("JT_NULL\n");
                break;
                
            case 't':
                if(json_parse_true(&parser))
                {
                    // error
                    return 1;
                }
                printf("JT_TRUE\n");
                break;
                
            case 'f':
                if(json_parse_false(&parser))
                {
                    // error
                    return 1;
                }
                printf("JT_FALSE");
                break;
            
            case ' ': case '\t': case '\n': case '\r':
                // skip whitespace
                break;
                
            default:
                break;
        }
    }
    
    return 0;
}

