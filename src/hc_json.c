/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/

#include <string.h>

#include <cjson/cJSON.h>

#include "hc_json.h"

/**************************************************************************************************/

static const char* hc_json_name_sensor_id = "sensorID";
static const char* hc_json_name_type = "type";
static const char* hc_json_name_temperature = "temperature";
static const char* hc_json_name_value = "value";
static const char* hc_json_name_motion = "motion";

/**************************************************************************************************/

static int32_t hc_json_parse_sensor_id(cJSON* json_obj, hc_sensor_sample* sample)
{
    int32_t res = -1;
    cJSON* json_item = cJSON_GetObjectItem(json_obj, hc_json_name_sensor_id);

    if(json_item != NULL && json_item->type == cJSON_String)
    {
        /* Sensor ID will be silently truncated to HC_SENSOR_NAME_MAX_LENGTH */
        strncpy(sample->name, json_item->valuestring, HC_SENSOR_NAME_MAX_LENGTH);
        /* Guarantee 0-termination */
        sample->name[HC_SENSOR_NAME_MAX_LENGTH] = '\0';
        res = 0;
    }

    return res;
}

/**************************************************************************************************/

static int32_t hc_json_parse_sensor_type(cJSON* json_obj, hc_sensor_sample* sample)
{
    int32_t res = -1;
    cJSON* json_item = cJSON_GetObjectItem(json_obj, hc_json_name_type);

    if(json_item != NULL && json_item->type == cJSON_String)
    {
        if(strcmp(json_item->valuestring, hc_json_name_temperature) == 0)
        {
            sample->type = HC_SENSOR_TYPE_TEMP;
            res = 0;
        }
        else if(strcmp(json_item->valuestring, hc_json_name_motion) == 0)
        {
            sample->type = HC_SENSOR_TYPE_MOTION;
            res = 0;
        }
    }

    return res;
}

/**************************************************************************************************/

static int32_t hc_json_parse_sensor_value(cJSON* json_obj, hc_sensor_sample* sample)
{
    int32_t res = -1;
    cJSON* json_item = cJSON_GetObjectItem(json_obj, hc_json_name_value);

    if(json_item != NULL)
    {
        switch(sample->type)
        {
            case HC_SENSOR_TYPE_TEMP:
            {
                if(json_item->type == cJSON_Number)
                {
                    /* Temperature is double in json. Convert, as only using ints internally */
                    sample->data.temp.temp = (int32_t)(json_item->valuedouble * 1000);
                    res = 0;
                }
            }
            break;

            case HC_SENSOR_TYPE_MOTION:
            {
                if(json_item->type == cJSON_True)
                {
                    sample->data.motion.motion = true;
                    res = 0;
                }
                else if(json_item->type == cJSON_False)
                {
                    sample->data.motion.motion = false;
                    res = 0;
                }
            }
            break;

            default:
                break;
        }
    }

    return res;
}

/**************************************************************************************************/

int32_t hc_json_parse_sensor_data(const char* json_string, hc_sensor_sample* sample)
{
    cJSON* json_obj;
    int32_t res = -1;

    json_obj = cJSON_Parse(json_string);

    if(json_obj != NULL)
    {
        res = hc_json_parse_sensor_id(json_obj, sample);

        if(res == 0)
        {
            res = hc_json_parse_sensor_type(json_obj, sample);
        }

        if(res == 0)
        {
            res = hc_json_parse_sensor_value(json_obj, sample);
        }

        cJSON_Delete(json_obj);
    }

    return res;
}

/**************************************************************************************************/

char* hc_json_create_valve_msg(int32_t valve_level)
{
    char* res = NULL;
    cJSON* json_obj = cJSON_CreateObject();

    if(json_obj != NULL)
    {
        cJSON_AddNumberToObject(json_obj, "level", (valve_level / 1000.0));
        res = cJSON_PrintUnformatted(json_obj);
        cJSON_Delete(json_obj);
    }

    return res;
}
