/**************************************************************************************************
 * Copyright (C) 2016-2017, Binarbaum LLC. All rights reserved.                                   *
 **************************************************************************************************/

/**
 * @file arrlist.c
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef ASSERT_ENABLED
#include <assert.h>
#define ARRLIST_ASSERT(COND) assert(COND)
#else
#define ARRLIST_ASSERT(COND)
#endif

#include <string.h>

#include <arrlist.h>

#define GET_INDEX(L, I) (void*)(L->data + ((I) * list->elem_size))

static int32_t arrlist_inc(arrlist_t* list);

/**************************************************************************************************/

int32_t arrlist_init(arrlist_t* list, uint16_t cap, uint16_t inc, uint16_t elem_size)
    {
    ARRLIST_ASSERT(list != NULL);
    ARRLIST_ASSERT(cap > 0);
    ARRLIST_ASSERT(elem_size > 0);

    list->data = ARRLIST_MALLOC(cap * elem_size);

    if(list->data != NULL)
        {
        list->cap = cap;
        list->inc = inc;
        list->elems = 0;
        list->elem_size = elem_size;
        list->free_fn = NULL;
        return ARRLIST_OK;
        }

    return ARRLIST_ERR_OOM;
    }

/**************************************************************************************************/

void arrlist_release(arrlist_t* list)
    {
    ARRLIST_ASSERT(list != NULL);
    arrlist_clear(list);
    ARRLIST_FREE(list->data);
    list->data = NULL;
    list->cap = 0;
    }

/**************************************************************************************************/

void arrlist_clear(arrlist_t* list)
    {
    ARRLIST_ASSERT(list != NULL);

    if(list->free_fn != NULL)
        {
        uint16_t i;

        for(i = 0; i < list->elems; ++i)
            {
            list->free_fn(GET_INDEX(list, i));
            }
        }

    list->elems = 0;
    }

/**************************************************************************************************/

void* arrlist_get_at(arrlist_t* list, uint16_t index)
    {
    ARRLIST_ASSERT(list != NULL);
    ARRLIST_ASSERT(list->elems > index);
    return GET_INDEX(list, index);
    }

/**************************************************************************************************/

int32_t arrlist_insert_at(arrlist_t* list, uint16_t index, void* el)
    {
    int32_t res = ARRLIST_OK;

    ARRLIST_ASSERT(list != NULL);
    ARRLIST_ASSERT(list->elems >= index);
    ARRLIST_ASSERT(el != NULL);

    if(list->elems < list->cap || (res = arrlist_inc(list)) == ARRLIST_OK)
        {
        if(index < list->elems)
            {
            memmove(GET_INDEX(list, index + 1),
                    GET_INDEX(list, index),
                    (list->elems - index) * list->elem_size);
            }

        memcpy(GET_INDEX(list, index), el, list->elem_size);
        list->elems++;
        }

    return res;
    }

/**************************************************************************************************/

static int32_t arrlist_inc(arrlist_t* list)
    {
    ARRLIST_ASSERT(list != NULL);

    if(list->inc == 0)
        {
        return ARRLIST_ERR_FULL;
        }
    else
        {
        uint8_t* new_data = ARRLIST_REALLOC(list->data, (list->cap + list->inc) * list->elem_size);

        if(new_data != NULL)
            {
            list->data = new_data;
            list->cap += list->inc;
            return ARRLIST_OK;
            }

        return ARRLIST_ERR_OOM;
        }
    }
