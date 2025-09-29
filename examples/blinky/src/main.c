/*
 * Copyright (c) 2025 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(blinky, LOG_LEVEL_DBG);

#include <golioth/client.h>
#include <golioth/rpc.h>
#include <samples/common/sample_credentials.h>
#include <samples/common/net_connect.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static K_SEM_DEFINE(connected, 0, 1);

static enum golioth_rpc_status on_light_on(zcbor_state_t *request_params_array,
                                           zcbor_state_t *response_detail_map,
                                           void *callback_arg)
{
    gpio_pin_set_dt(&led, 1);
    LOG_DBG("light on");

    return GOLIOTH_RPC_OK;
}

static enum golioth_rpc_status on_light_off(zcbor_state_t *request_params_array,
                                            zcbor_state_t *response_detail_map,
                                            void *callback_arg)
{
    gpio_pin_set_dt(&led, 0);
    LOG_DBG("light off");

    return GOLIOTH_RPC_OK;
}

static void on_client_event(struct golioth_client *client,
                            enum golioth_client_event event,
                            void *arg)
{
    bool is_connected = (event == GOLIOTH_CLIENT_EVENT_CONNECTED);
    if (is_connected)
    {
        k_sem_give(&connected);
    }
    LOG_INF("Golioth client %s", is_connected ? "connected" : "disconnected");
}

int main(void)
{
    LOG_DBG("Start tinymcp blinky");

    int err;

    if (!device_is_ready(led.port))
    {
        return -EIO;
    }

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (err < 0)
    {
        return -EIO;
    }

    net_connect();

    /* Note: In production, credentials should be saved in secure storage. For
     * simplicity, we provide a utility that stores credentials as plaintext
     * settings.
     */
    const struct golioth_client_config *client_config = golioth_sample_credentials_get();

    struct golioth_client *client = golioth_client_create(client_config);
    struct golioth_rpc *rpc = golioth_rpc_init(client);

    golioth_client_register_event_callback(client, on_client_event, NULL);

    k_sem_take(&connected, K_FOREVER);

    err = golioth_rpc_register(rpc, "light_on", on_light_on, NULL);
    if (err)
    {
        LOG_ERR("Failed to register light_on RPC: %d", err);
    }

    err = golioth_rpc_register(rpc, "light_off", on_light_off, NULL);
    if (err)
    {
        LOG_ERR("Failed to register light_off RPC: %d", err);
    }

    while (true)
    {
        k_sleep(K_SECONDS(5));
    }

    return 0;
}
