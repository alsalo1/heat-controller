/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/
#pragma once

#include "hc_sensor.h"

int32_t hc_json_parse_sensor_data(const char* json_string, hc_sensor_sample* sample);

char* hc_json_create_valve_msg(int32_t valve_level);
