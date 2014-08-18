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
    JT_NUMBER,
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
    struct json_token last_token;
};

static void json_parser_start_object()
{
    printf("JL_LBRACE\n");
}

static void json_parser_finish_object()
{
    printf("JL_RBRACE\n");
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
        
        if(c == '\"')
        {
            return 0;
        }
        
        if(c == '\\')
        {
            parser->last_token.has_escape = 1;
            parser->cur++;
            switch (*parser->cur) {
                case '\"': case '/': case 'b': case 't':
                case '\\': case 'f': case 'r': case 'n':
                    break;
                    
                case 'u':
                    parser->cur++;
                    int i = 0;
                    break;
                    
                default:
                    return 1; // TODO: return error invalid token
                    break;
            }
        }
    }
    
    return 0; // TODO: return error partial json
}

int json_parser_parse(const char *json, size_t nlength)
{
    struct json_parser parser;
    parser.cur = json;
    parser.end = json + nlength;
    
    for(;parser.cur != parser.end; ++parser.cur)
    {
        char ch = *parser.cur;
        switch (ch) {
            case JL_LBRACE:
                json_parser_start_object();
                break;
                
            case JL_RBRACE:
                json_parser_finish_object();
                break;
                
            case JL_DQUOT:
                json_parse_string(&parser);
                break;
                
            case JL_COLON:
                printf("JL_COLON ");
                break;
                
            case JL_COMMA:
                printf("JL_COMMA");
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

