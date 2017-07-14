/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/

#include <stdio.h>
#include <unistd.h>

#include "hc_json.h"
#include "hc_mqtt.h"

typedef struct
{
    hc_mqtt mqtt;
} hc_main;

/**************************************************************************************************/

static hc_main hc;

static const char* hc_main_mqtt_broker_addr = "tcp://localhost:1883";

#define HC_MAIN_ADJUST_LOOP_IN_S 10

/**************************************************************************************************/

static void hc_main_msg_handler(void* ctx, const char* msg)
{
    hc_sensor_sample sample;
    int32_t res;

    printf("[MQTT]: %s\n", msg);

    res = hc_json_parse_sensor_data(msg, &sample);

    if(res == 0)
    {
        printf("[SENSOR SAMPLE]: name %s, temp: %d\n", sample.name, sample.data.temp.temp);
    }
}

/**************************************************************************************************/

int main(int argc, char** argv)
{
    int32_t res;

    res = hc_mqtt_init(&hc.mqtt, hc_main_msg_handler, &hc);

    if(res == 0)
    {
        res = hc_mqtt_conn_and_sub(&hc.mqtt, hc_main_mqtt_broker_addr);
        
        while(res == 0)
        {
            sleep(HC_MAIN_ADJUST_LOOP_IN_S);
        }
    }

    return (int)res;
}
