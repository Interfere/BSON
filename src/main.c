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

#include <stdlib.h>
#include <stdio.h>
#include "oid.h"
#include "cpl_array.h"
#include "documentbuilder.h"
#include "document.h"
#include "iterator.h"

static inline void test_oid()
{
    bson_oid_ref oid = bson_oid_create();
    bson_oid_init(oid);
    char* representation = bson_oid_string_create(oid);
    bson_oid_destroy(oid);
    
    puts(representation);
    free(representation);
    
    oid = bson_oid_create();
    bson_oid_init_sequential(oid);
    representation = bson_oid_string_create(oid);
    bson_oid_destroy(oid);
    
    puts(representation);
    free(representation);
}

static inline void test_cpl_array()
{
    cpl_array_ref a = cpl_array_create(sizeof(int), 4);
    
    int i = 0;
    cpl_array_push_back(a, i);
    i = 1;
    cpl_array_push_back(a, i);
    i = 2;
    cpl_array_push_back(a, i);
    i = 3;
    cpl_array_push_back(a, i);
    i = 4;
    cpl_array_push_back(a, i);
    i = 5;
    cpl_array_push_back(a, i);
    i = 6;
    cpl_array_push_back(a, i);
    i = 7;
    cpl_array_push_back(a, i);
    
    int* arr = cpl_array_data(a, int);
    for (int k = 0; k < cpl_array_count(a); ++k) {
        printf("a[%d] = %d\n", k, arr[k]);
        int it = cpl_array_get(a, k, int);
        printf("b[%d] = %d\n", k, it);
    }
    
    cpl_array_destroy(a);
}

static inline void test_builder()
{
    char* argv[4] = {
        "index1",
        "index2",
        "index3",
        "index4"
    };
    bson_array_builder_ref ab = bson_array_builder_create();
    for (int i = 0; i < 4; i++) {
        bson_document_builder_ref d = bson_document_builder_create();
        
        bson_document_builder_append_str(d, "name", argv[i]);
        bson_document_builder_append_i(d, "page", i+2);
        
        bson_document_ref doc = bson_document_builder_finalize(d);
        bson_array_builder_append_doc(ab, doc);
        bson_document_destroy(doc);
    }
    
    bson_array_ref indicies = bson_array_builder_finalize(ab);
    
    bson_document_builder_ref b = bson_document_builder_create();
    
    bson_oid_ref oid = bson_oid_create();
    bson_oid_init(oid);
    bson_document_builder_append_oid(b, "_id", oid);
    bson_oid_destroy(oid);
    
    bson_document_builder_append_str(b, "title", "test_table");
    bson_document_builder_append_b(b, "isActive", 1);
    bson_document_builder_append_arr(b, "indicies", indicies);
    bson_array_destroy(indicies);
    
    bson_document_ref d = bson_document_builder_finalize(b);
    printf("sizeof d: %d\n", bson_document_size(d));
    
    bson_iterator_t iter;
    bson_element_ref el = 0;
    for (el = bson_iterator_init(&iter, d); !bson_iterator_end(&iter); bson_iterator_next_el(&iter, el)) {
        printf("Element: size=%zu key=%s", bson_element_size(el), bson_element_fieldname(el));
        
        switch(bson_element_type(el))
        {
            case bson_type_string:
                printf(" value=[%d]%s", *(int32_t*)bson_element_value(el), bson_element_value(el) + sizeof(int32_t));
                break;
            
            case bson_type_bool:
                printf(" value=%s", (*bson_element_value(el)?"true":"false"));
                break;
                
            case bson_type_oid:
            {
                char* oid = bson_oid_string_create((const bson_oid_ref)bson_element_value(el));
                printf(" value=%s", oid);
                free(oid);
                break;
            }
                
            case bson_type_array:
            {
                bson_array_ref arr = bson_array_create_with_data(bson_element_value(el));
                printf(" values = [%d]{\n", bson_document_size(arr));
                bson_iterator_t i;
                bson_element_ref e = 0;
                for(e = bson_iterator_init(&i, arr); !bson_iterator_end(&i); bson_iterator_next_el(&i, e))
                {
                    printf("    Element: size=%zu key=%s\n", bson_element_size(e), bson_element_fieldname(e));
                }
                bson_element_destroy(e);
                bson_array_destroy(arr);
                printf("};");
                break;
            }
        }
        
        putchar('\n');
    }
    
    bson_element_destroy(el);
    
    bson_document_destroy(d);
}

int main(int argc, char* argv[])
{
    test_oid();
    
    test_cpl_array();
    
    test_builder();
    
    return EXIT_SUCCESS;
}
