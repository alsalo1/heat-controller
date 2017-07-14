/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/

#include <string.h>

#include <cjson/cJSON.h>

#include "hc_json.h"

int32_t hc_json_parse_sensor_data(const char* json_string, hc_sensor_sample* sample)
{
    cJSON* json_obj;
    int32_t res = -1;

    json_obj = cJSON_Parse(json_string);

    if(json_obj != NULL)
    {
        cJSON* json_item = cJSON_GetObjectItem(json_obj, "sensorID");

        if(json_item != NULL && json_item->type == cJSON_String)
        {
            strncpy(sample->name, json_item->valuestring, HC_SENSOR_NAME_MAX_LENGTH);
            sample->name[HC_SENSOR_NAME_MAX_LENGTH] = '\0';
            res = 0;
        }

        if(res == 0)
        {
            res = -1;
            json_item = cJSON_GetObjectItem(json_obj, "type");

            if(json_item != NULL && json_item->type == cJSON_String)
            {
                if(strcmp(json_item->valuestring, "temperature") == 0)
                {
                    sample->type = HC_SENSOR_TYPE_TEMP;
                    res = 0;
                }
            }
        }

        if(res == 0)
        {
            res = -1;

            if(sample->type == HC_SENSOR_TYPE_TEMP)
            {
                json_item = cJSON_GetObjectItem(json_obj, "value");

                if(json_item != NULL && json_item->type == cJSON_Number)
                {
                    sample->data.temp.temp = (int32_t)(json_item->valuedouble * 1000);
                    res = 0;
                }
            }
        }

        cJSON_Delete(json_obj);
    }

    return res;
}
