/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/
#pragma once

#include "hc_sensor.h"

/**
 * Parse stringified json object to sensor sample structure
 *
 * @param[in] json_string Stringified json object
 * @param[out] sample Sample to write parsed data to
 * @return 0 in case of success, negative in case of error
 */
int32_t hc_json_parse_sensor_data(const char* json_string, hc_sensor_sample* sample);

/**
 * Create a stringified json object for valve control
 *
 * @param[in] valve_level Valve level in parts per hundred thousand (percent times 1000)
 * @return Stringified json object, NULL in case of error.
           Caller is responsible for deallocating the created string
 */
char* hc_json_create_valve_msg(int32_t valve_level);
