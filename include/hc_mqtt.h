/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/
#pragma once

#include <stdint.h>

#include <MQTTClient.h>

typedef void (*hc_mqtt_msg_cb)(void* ctx, const char* msg);

typedef struct
{
    MQTTClient client;
    hc_mqtt_msg_cb cb;
    void* cb_ctx;
} hc_mqtt;

/**
 * Initialize MQTT client
 */
int32_t hc_mqtt_init(hc_mqtt* obj, hc_mqtt_msg_cb cb, void* cb_ctx);

/**
 * Free all resources allocated by MQTT client
 */
int32_t hc_mqtt_free(hc_mqtt* obj);

/**
 * Connect client to a broker and subscribe to sensor readings
 */
int32_t hc_mqtt_conn_and_sub(hc_mqtt* obj, const char* addr);

/**
 * Disconnect client from a broker
 */
int32_t hc_mqtt_disconnect(hc_mqtt* obj);

/**
 * Publish a new valve setting
 */
int32_t hc_mqtt_pub_adjust(hc_mqtt* obj, const char* msg);
