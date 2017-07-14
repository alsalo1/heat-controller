/**************************************************************************************************
 * Copyright (C) 2016-2017, Binarbaum LLC. All rights reserved.                                   *
 **************************************************************************************************/

/**
 * @file arrlist.h
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#if !defined ARRLIST_MALLOC || !defined ARRLIST_REALLOC || !defined ARRLIST_FREE

#include <stdlib.h>

#ifndef ARRLIST_MALLOC
#define ARRLIST_MALLOC malloc
#endif

#ifndef ARRLIST_REALLOC
#define ARRLIST_REALLOC realloc
#endif

#ifndef ARRLIST_FREE
#define ARRLIST_FREE free
#endif

#endif /* !defined ARRLIST_MALLOC || !defined ARRLIST_REALLOC || !defined ARRLIST_FREE */

/**
 * @brief Free all resources allocated by a list element
 * @param[in] el List element
 * @pre el != NULL
 */
typedef void (*arrlist_el_free)(void* el);

typedef struct
    {
    uint16_t cap;            /**< @brief Allocated list capacity */
    uint16_t inc;            /**< @brief Increment to allocate when list has reached capacity */
    uint16_t elems;          /**< @brief Elements in list */
    uint16_t elem_size;      /**< @brief Element size in bytes */
    arrlist_el_free free_fn; /**< @brief Element free function */
    uint8_t* data;           /**< @brief Element data buffer */
    } arrlist_t;

#define ARRLIST_OK        0 /**< @brief OK */
#define ARRLIST_ERR_OOM  -1 /**< @brief Error, out of memory */
#define ARRLIST_ERR_FULL -2 /**< @brief Error, list is full */

/**
 * Initialize a list
 * @param[in,out] list      List to initialize
 * @param[in]     cap       Initial list capacity
 * @param[in]     inc       Capacity to add when current limit reached
 * @param[in]     elem_size Element size in bytes
 * @return ARRLIST_OK if successful
 */
int32_t arrlist_init(arrlist_t* list, uint16_t cap, uint16_t inc, uint16_t elem_size);

/**
 * @brief Release all resources allocated by a list.
 * All elements are free'd, internal data buffer is released and list capacity is set to 0.
 * @param[in,out] list List to release
 */
void arrlist_release(arrlist_t* list);

/**
 * @brief Clear a list and free all elements in it. Doesn't change list capacity
 * @param[in,out] list List to clear
 */
void arrlist_clear(arrlist_t* list);

void* arrlist_get_at(arrlist_t* list, uint16_t index);

#define arrlist_get_first(/* arrlist_t* */l) arrlist_get_at((l), 0)

#define arrlist_get_last(/* arrlist_t* */l) arrlist_get_at((l), ((l)->elems - 1))

/**
 * Insert element to a list
 * @param list  List to add element to
 * @param index Position to add element to
 * @param el    Element to add
 * @return ARRLIST_OK if successful
 */
int32_t arrlist_insert_at(arrlist_t* list, uint16_t index, void* el);

/**
 * Add element to beginning of a list
 */
#define arrlist_insert_first(/* arrlist_t* */l, /* void* */el) arrlist_insert_at((l), 0, (el))

/**
 * Add element to end of a list
 */
#define arrlist_insert_last(/* arrlist_t* */l, /* void* */el) \
    arrlist_insert_at((l), (l)->elems, (el))

/**
 * Set method for freeing list elements
 */
#define arrlist_set_free_fn(/* arrlist_t* */ list, /* arrlist_el_free */ fn) \
    (list)->free_fn = fn

/**
 * Check if list @a l is empty
 */
#define arrlist_empty(/* arrlist_t* */l) ((l)->elems == 0) ? true : false

/**
 * Returns number of elements in @a l list
 */
#define arrlist_elems(/* arlist_t* */l) (l)->elems
