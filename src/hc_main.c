/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/

#include <stdio.h>
#include <unistd.h>

#include <pthread.h>

#include "arrlist.h"
#include "hc_json.h"
#include "hc_mqtt.h"

typedef struct
{
    hc_mqtt mqtt;
    arrlist_t samples;
    pthread_mutex_t lock;
    uint32_t valve_level;
} hc_main;

/**************************************************************************************************/

static hc_main hc;

static const char* hc_main_mqtt_broker_addr = "tcp://localhost:1883";

#define HC_MAIN_ADJUST_LOOP_IN_S 10

#define HC_MAIN_TARGET_TEMP 22000

#define HC_MAIN_TARGET_DIFF 500

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
        pthread_mutex_lock(&hc.lock);
        arrlist_insert_last(&hc.samples, &sample);
        pthread_mutex_unlock(&hc.lock);
    }
}

/**************************************************************************************************/

static int32_t hc_main_adjust_valve()
{
    int32_t res = 0;

    pthread_mutex_lock(&hc.lock);

    if(!arrlist_empty(&hc.samples))
    {
        uint16_t i;
        int32_t temp_total = 0;
        char* valve_msg;

        for(i = 0; i < arrlist_elems(&hc.samples); ++i)
        {
            hc_sensor_sample* sample = (hc_sensor_sample*)arrlist_get_at(&hc.samples, i);
            temp_total += sample->data.temp.temp;
        }

        temp_total /= i;

        if(temp_total >= HC_MAIN_TARGET_TEMP + HC_MAIN_TARGET_DIFF)
        {
            hc.valve_level = 0;
        }
        else if(temp_total <= HC_MAIN_TARGET_TEMP - HC_MAIN_TARGET_DIFF)
        {
            hc.valve_level = 100000;
        }

        valve_msg = hc_json_create_valve_msg(hc.valve_level);

        if(valve_msg != NULL)
        {
            res = hc_mqtt_pub_adjust(&hc.mqtt, valve_msg);
            free(valve_msg);
        }

        arrlist_clear(&hc.samples);
    }

    pthread_mutex_unlock(&hc.lock);
    return res;
} 

/**************************************************************************************************/

int main(int argc, char** argv)
{
    int32_t res;

    hc.valve_level = 50000;

    res = arrlist_init(&hc.samples, 10, 10, sizeof(hc_sensor_sample));

    if(res == 0)
    {
        pthread_mutexattr_t attr;

        pthread_mutexattr_init(&attr);
        res = pthread_mutex_init(&hc.lock, &attr);
    }

    if(res == 0)
    {
        res = hc_mqtt_init(&hc.mqtt, hc_main_msg_handler, &hc);
    }

    if(res == 0)
    {
        res = hc_mqtt_conn_and_sub(&hc.mqtt, hc_main_mqtt_broker_addr);
        
        while(res == 0)
        {
            res = hc_main_adjust_valve();
            sleep(HC_MAIN_ADJUST_LOOP_IN_S);
        }
    }

    return (int)res;
}
