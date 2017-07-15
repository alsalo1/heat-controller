/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hc_mqtt.h"

/**************************************************************************************************/

#define HC_MQTT_DISCONNECT_TIMEOUT_MS 10000

static const char* hc_mqtt_client_id = "heat-controller";

static const char* hc_mqtt_topic_temp_reading = "/readings/temperature";

static const char* hc_mqtt_topic_motion_reading = "/readings/motion";

static const char* hc_mqtt_topic_all_readings = "/readings/+";

static const char* hc_mqtt_topic_act_room1 = "/actuators/room-1";

/**************************************************************************************************/

static void hc_mqtt_conn_lost(void* ctx, char* reason)
{
}

/**************************************************************************************************/

static void hc_mqtt_msg_delivered(void* ctx, MQTTClient_deliveryToken token)
{
}

/**************************************************************************************************/

static int hc_mqtt_msg_received(void* ctx, char* topic, int topic_len, MQTTClient_message* msg)
{
    hc_mqtt* obj = (hc_mqtt*)ctx;

    if(obj->cb != NULL &&
       (strcmp(topic, hc_mqtt_topic_temp_reading) == 0 ||
        strcmp(topic, hc_mqtt_topic_motion_reading) == 0))
    {
        char* msg_string = malloc(msg->payloadlen + 1);

        if(msg_string != NULL)
        {
            memcpy(msg_string, msg->payload, msg->payloadlen);
            msg_string[msg->payloadlen] = '\0';
            obj->cb(obj->cb_ctx, msg_string);
            free(msg_string);
        }
    }

    MQTTClient_freeMessage(&msg);
    MQTTClient_free(topic);
    return true;
}

/**************************************************************************************************/

int32_t hc_mqtt_init(hc_mqtt* obj, hc_mqtt_msg_cb cb, void* cb_ctx)
{
    int32_t res = -1;

    if(obj != NULL)
    {
        obj->client = NULL;
        obj->cb = cb;
        obj->cb_ctx = cb_ctx;
        res = 0;
    }

    return res;
}

/**************************************************************************************************/

int32_t hc_mqtt_free(hc_mqtt* obj)
{
    int32_t res = -1;

    if(obj != NULL)
    {
        if(obj->client != NULL)
        {
            if(MQTTClient_isConnected(obj->client))
            {
                MQTTClient_disconnect(obj->client, HC_MQTT_DISCONNECT_TIMEOUT_MS);
            }

            MQTTClient_destroy(&obj->client);
            obj->client = NULL;
        }

        res = 0;
    }

    return res;
}

/**************************************************************************************************/

int32_t hc_mqtt_conn_and_sub(hc_mqtt* obj, const char* addr)
{
    int32_t res = -1;

    if(obj != NULL)
    {
        MQTTClient_connectOptions conn_opt = MQTTClient_connectOptions_initializer;

        if(obj->client == NULL)
        {
            res = MQTTClient_create(&obj->client,
                                    addr,
                                    hc_mqtt_client_id,
                                    MQTTCLIENT_PERSISTENCE_NONE,
                                    NULL);

            if(res != MQTTCLIENT_SUCCESS)
            {
                goto hc_mqtt_exit;
            }
        }

        res = MQTTClient_setCallbacks(obj->client,
                                      obj,
                                      hc_mqtt_conn_lost,
                                      hc_mqtt_msg_received,
                                      hc_mqtt_msg_delivered);

        if(res != MQTTCLIENT_SUCCESS)
        {
            goto hc_mqtt_exit;
        }

        conn_opt.keepAliveInterval = 30;
        conn_opt.cleansession = 1;

        res = MQTTClient_connect(obj->client, &conn_opt);

        if(res != MQTTCLIENT_SUCCESS)
        {
            goto hc_mqtt_exit;
        }

        res = MQTTClient_subscribe(obj->client, hc_mqtt_topic_all_readings, 1);

 hc_mqtt_exit:
        if(res != MQTTCLIENT_SUCCESS)
        {
            hc_mqtt_free(obj);
        }
    }

    return res;
}

/**************************************************************************************************/

int32_t hc_mqtt_disconnect(hc_mqtt* obj)
{
    int32_t res = -1;

    if(obj != NULL && obj->client != NULL && MQTTClient_isConnected(obj->client))
    {
        res = MQTTClient_disconnect(obj->client, HC_MQTT_DISCONNECT_TIMEOUT_MS);
    }

    return res;
}

/**************************************************************************************************/

int32_t hc_mqtt_pub_adjust(hc_mqtt* obj, const char* msg)
{
    int32_t res = -1;

    if(obj != NULL && obj->client != NULL && MQTTClient_isConnected(obj->client))
    {
        res = MQTTClient_publish(obj->client,
                                 hc_mqtt_topic_act_room1,
                                 strlen(msg),
                                 (void*)msg,
                                 1,
                                 0,
                                 NULL);
    }

    return res;
}
