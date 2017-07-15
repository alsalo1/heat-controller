/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    HC_SENSOR_TYPE_TEMP = 0, /**< Temperature sensor */
    HC_SENSOR_TYPE_MOTION /**< Motion sensor */
} hc_sensor_type;

typedef struct
{
    int32_t temp; /**< Temperature in centigrade * 1000 */
} hc_sensor_temp_data;

typedef struct
{
    bool motion; /**< Is motion detected */
} hc_sensor_motion_data;

typedef union
{
    hc_sensor_temp_data temp;
    hc_sensor_motion_data motion;
} hc_sensor_data;

#define HC_SENSOR_NAME_MAX_LENGTH 32

typedef struct
{
    char name[HC_SENSOR_NAME_MAX_LENGTH + 1];
    hc_sensor_type type;
    hc_sensor_data data;
} hc_sensor_sample;
