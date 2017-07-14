/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/
#pragma once

#include <stdint.h>

typedef enum
{
    HC_SENSOR_TYPE_TEMP = 0
} hc_sensor_type;

typedef struct
{
    int32_t temp;
} hc_sensor_temp_data;

typedef union
{
    hc_sensor_temp_data temp;
} hc_sensor_data;

#define HC_SENSOR_NAME_MAX_LENGTH 32

typedef struct
{
    char name[HC_SENSOR_NAME_MAX_LENGTH + 1];
    hc_sensor_type type;
    hc_sensor_data data;
} hc_sensor_sample;
