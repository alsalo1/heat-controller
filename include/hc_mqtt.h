/**************************************************************************************************
 * Copyright (C) 2017, Aleksi Salo. All rights reserved.                                          *
 **************************************************************************************************/
#pragma once

#include <stdint.h>

#include <MQTTClient.h>

typedef struct
{
    MQTTClient client;
} hc_mqtt;

int32_t hc_mqtt_init(hc_mqtt* obj);

int32_t hc_mqtt_free(hc_mqtt* obj);

int32_t hc_mqtt_conn_and_sub(hc_mqtt* obj, const char* addr);

int32_t hc_mqtt_disconnect(hc_mqtt* obj);
