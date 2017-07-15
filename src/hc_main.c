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
    hc_mqtt mqtt; /**< MQTT client */
    arrlist_t temp_samples; /**< Received temperature samples */
    arrlist_t motion_samples; /**< Received motion samples */
    pthread_mutex_t lock;
    uint32_t valve_level; /**< Current valve level */
    uint32_t no_motion_count; /**< Adjustment loops without motion detected */
    int32_t last_temp; /**< Last known temperature */
} hc_main;

/**************************************************************************************************/

static hc_main hc;

static const char* hc_main_mqtt_broker_addr = "tcp://localhost:1883";

#define HC_MAIN_ADJUST_LOOP_IN_S 10 /**< How often to adjust valve */

#define HC_MAIN_TARGET_TEMP 22000 /**< Target temperature (C * 1000) */

#define HC_MAIN_TARGET_DIFF 500 /**< Allowed temperature tolerance (C * 1000) */

#define HC_MAIN_NO_MOTION_TRIGGER_IN_S 60 /**< Turn heating off if no motion in this time */

#define HC_MAIN_UNKNOWN_TEMP INT32_MAX

/**************************************************************************************************/

static void hc_main_msg_handler(void* ctx, const char* msg)
{
    hc_sensor_sample sample;
    int32_t res;

    printf("[MQTT]: %s\n", msg);

    res = hc_json_parse_sensor_data(msg, &sample);

    if(res == 0)
    {
        /* This is not called from the same thread as main loop -> use mutex */
        pthread_mutex_lock(&hc.lock);

        switch(sample.type)
        {
            case HC_SENSOR_TYPE_TEMP:
            {
                printf("[SENSOR SAMPLE]: name %s, temp: %d\n", sample.name, sample.data.temp.temp);        
                /* For time being, don't care about sensor name. All temps go to same array */
                arrlist_insert_last(&hc.temp_samples, &sample);
            }
            break;

            case HC_SENSOR_TYPE_MOTION:
            {
                printf("[SENSOR SAMPLE]: name %s, motion: %d\n",
                       sample.name,
                       sample.data.motion.motion);
                /* For time being, don't care about sensor name. All motions go to same array */
                arrlist_insert_last(&hc.motion_samples, &sample);
            }
            break;

            default:
                break;
        }

        pthread_mutex_unlock(&hc.lock);
    }
}

/**************************************************************************************************/

static bool hc_main_is_motion()
{
    /* Do we have new motion readings */
    if(!arrlist_empty(&hc.motion_samples))
    {
        uint16_t i;

        /* Check for motion */
        for(i = 0; i < arrlist_elems(&hc.motion_samples); ++i)
        {
            hc_sensor_sample* sample = (hc_sensor_sample*)arrlist_get_at(&hc.motion_samples, i);

            if(sample->data.motion.motion)
            {
                /* Motion reported, reset counter */
                hc.no_motion_count = 0;
            }
        }

        if(i == arrlist_elems(&hc.motion_samples))
        {
            /* No motion, increment counter */
            hc.no_motion_count++;
        }
    }
    else
    {
        /* No new motion readings, assume it means no motion and increment */
        hc.no_motion_count++;
    }

    return ((hc.no_motion_count * HC_MAIN_ADJUST_LOOP_IN_S) < HC_MAIN_NO_MOTION_TRIGGER_IN_S);
}

/**************************************************************************************************/

static int32_t hc_main_adjust_valve()
{
    int32_t res = 0;
    uint32_t new_valve_level = hc.valve_level;
    bool motion;

    pthread_mutex_lock(&hc.lock);

    motion = hc_main_is_motion();

    if(!motion)
    {
        /* No motion. Close valve if not done already. */
        if(hc.valve_level > 0)
        {
            new_valve_level = 0;
        }
    }

    /* Check new temperature readings */
    if(!arrlist_empty(&hc.temp_samples))
    {
        uint16_t i;

        hc.last_temp = 0;

        /* Calculate average of received temperature readings and use that for adjustment */
        for(i = 0; i < arrlist_elems(&hc.temp_samples); ++i)
        {
            hc_sensor_sample* sample = (hc_sensor_sample*)arrlist_get_at(&hc.temp_samples, i);
            hc.last_temp += sample->data.temp.temp;
        }

        hc.last_temp /= i;

        /* Adjust valve only when there has been recent movement */
        if(motion)
        {
            /* Temperature is over acceptable limit -> close the valve */
            if(hc.last_temp >= HC_MAIN_TARGET_TEMP + HC_MAIN_TARGET_DIFF)
            {
                new_valve_level = 0;
            }
            /* Temperature is below acceptable limit -> fully open the valve */
            else if(hc.last_temp <= HC_MAIN_TARGET_TEMP - HC_MAIN_TARGET_DIFF)
            {
                new_valve_level = 100000;
            }
        }
    }
    /* Motion detected but no new temp samples, adjust valve based on last known data. */
    else if(motion)
    {
        /* No need to check the other direction. There is only need to adjust in this branch
           when state changes from no-motion to motion (and no-motion has valve always off) */
        if(hc.last_temp != HC_MAIN_UNKNOWN_TEMP &&
           hc.last_temp <= HC_MAIN_TARGET_TEMP - HC_MAIN_TARGET_DIFF)
        {
            new_valve_level = 100000;
        }
    }

    /* Valve has been adjusted. Publish the change */
    if(new_valve_level != hc.valve_level)
    {
        char* valve_msg;

        hc.valve_level = new_valve_level;
        
        valve_msg = hc_json_create_valve_msg(hc.valve_level);

        if(valve_msg != NULL)
        {
            res = hc_mqtt_pub_adjust(&hc.mqtt, valve_msg);
            free(valve_msg);
        }
    }

    /* Clear the readings used in this adjustment cycle */
    arrlist_clear(&hc.motion_samples);
    arrlist_clear(&hc.temp_samples);

    pthread_mutex_unlock(&hc.lock);
    return res;
} 

/**************************************************************************************************/

int main(int argc, char** argv)
{
    int32_t res;

    hc.valve_level = 50000; /* Initialize valve level to 50% */
    hc.no_motion_count = 0; /* Assume motion in initial state */
    hc.last_temp = HC_MAIN_UNKNOWN_TEMP;

    /* Initialize sample storages */
    res = arrlist_init(&hc.temp_samples, 10, 10, sizeof(hc_sensor_sample));

    if(res == 0)
    {
        arrlist_init(&hc.motion_samples, 10, 10, sizeof(hc_sensor_sample));
    }

    /* Initialize mutex */
    if(res == 0)
    {
        pthread_mutexattr_t attr;

        pthread_mutexattr_init(&attr);
        res = pthread_mutex_init(&hc.lock, &attr);
    }

    /* Initialize MQTT client */
    if(res == 0)
    {
        res = hc_mqtt_init(&hc.mqtt, hc_main_msg_handler, &hc);
    }

    /* Connect the client and start running the adjustment loop */ 
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
