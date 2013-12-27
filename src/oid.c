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

#include "oid.h"

#include <stdlib.h>
#include <pthread.h>
#include "random.h"
#include "atomic.h"

/*********************** Private ObjectID properties **************************/
static int64_t s_sequential_inc = 0;

/*********************** Private ObjectID interface ***************************/
static void bson_oid_generate_machine_and_pid(struct machine_and_pid *__restrict p)
{
    
}

static void bson_oid_init(bson_oid_ref __restrict oid)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static int32_t inc = 0;
    static struct machine_and_pid machine_and_pid = { 0, 0 };
    if(inc == 0)
    {
        pthread_mutex_lock(&mutex);
        if(inc == 0)
        {
            inc = random_generate_next32();
            bson_oid_generate_machine_and_pid(&machine_and_pid);
        }
        pthread_mutex_unlock(&mutex);
    }
    
    {
        const time_t t = time(0);
        const uint8_t* T = (const uint8_t *)&t;
        oid->time[0] = T[3];
        oid->time[1] = T[2];
        oid->time[2] = T[1];
        oid->time[3] = T[0];
    }
    
    oid->machine_and_pid = machine_and_pid;
    
    {
        const int32_t new_inc = atomic_increment(&inc);
        const uint8_t *T = (const uint8_t *)&new_inc;
        oid->inc[0] = T[2];
        oid->inc[1] = T[1];
        oid->inc[2] = T[0];
    }
}

static void bson_oid_init_sequential(bson_oid_ref __restrict oid)
{
    {
        const time_t t = time(0);
        const uint8_t* T = (const uint8_t *)&t;
        oid->time[0] = T[3];
        oid->time[1] = T[2];
        oid->time[2] = T[1];
        oid->time[3] = T[0];
    }
    
    {
        const int64_t new_inc = atomic_increment64(&s_sequential_inc);
        const uint8_t *T = (const uint8_t *)&new_inc;
        oid->data[4] = T[7];
        oid->data[5] = T[6];
        oid->data[6] = T[5];
        oid->data[7] = T[4];
        oid->data[8] = T[3];
        oid->data[9] = T[2];
        oid->data[10] = T[1];
        oid->data[11] = T[0];
    }
}

/*********************** Public ObjectID interface ****************************/
bson_oid_ref bson_oid_create()
{
    bson_oid_ref oid = (bson_oid_ref)malloc(sizeof(struct bson_oid));
    if(oid)
    {
        bson_oid_init(oid);
    }
    return oid;
}

bson_oid_ref bson_oid_create_with_string(const char *__restrict string)
{
    bson_oid_ref oid = (bson_oid_ref)malloc(sizeof(struct bson_oid));
    // TODO: init with string
    return oid;
}

bson_oid_ref bson_oid_create_with_bytes(const uint8_t arr[bson_oid_size])
{
    bson_oid_ref oid = (bson_oid_ref)malloc(sizeof(struct bson_oid));
    if(oid)
    {
        memcpy(oid->data, arr, bson_oid_size);
    }
    return oid;
}

char* bson_oid_string_create(const bson_oid_ref oid)
{
    static const char hexchars[] = "0123456789abcdef";
    char* string = (char *)malloc(bson_oid_size * 2 + 1);
    
    if(string)
    {
        for (int i=0; i<bson_oid_size; ++i) {
            char c = oid->data[i];
            char hi = hexchars[(c & 0xF0) >> 4];
            char lo = hexchars[(c & 0x0F)];
            
            string[i*2] = hi;
            string[i*2 + 1] = lo;
        }
        string[bson_oid_size * 2] = '\0';
    }
    
    return string;
}
