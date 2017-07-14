/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/

#include <stdbool.h>

#include "hc_mqtt.h"

/**************************************************************************************************/

#define HC_MQTT_DISCONNECT_TIMEOUT_MS 10000

static const char* hc_mqtt_client_id = "heat-controller";

static const char* hc_mqtt_topic_all_readings = "/readings/+";

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
    MQTTClient_freeMessage(&msg);
    MQTTClient_free(topic);
    return true;
}

/**************************************************************************************************/

int32_t hc_mqtt_init(hc_mqtt* obj)
{
    int32_t res = -1;

    if(obj != NULL)
    {
        obj->client = NULL;
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
            MQTTClient_disconnect(obj->client, HC_MQTT_DISCONNECT_TIMEOUT_MS);
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

    if(obj != NULL && obj->client != NULL)
    {
        res = MQTTClient_disconnect(obj->client, HC_MQTT_DISCONNECT_TIMEOUT_MS);
    }

    return res;
}
