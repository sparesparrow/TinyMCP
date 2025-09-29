# blinky

This example demonstrates how to let an LLM control an LED on a device.

## Setup

This example uses [Zephyr](https://www.zephyrproject.org/), which requires a few
steps to get setup. See the [main `README.md`](../../README.md) for more
information.

You'll also need to apply the JSON object in [`state.json`](./state.json) to
your device's LightDB State to expose the RPC methods defined in the firmware
via the `tinymcp` MCP sever.

## Building & Flashing

While adding support for any hardware should be relatively straightforward,
building for the [nRF9160](https://www.nordicsemi.com/Products/nRF9160) is
currently supported without any additional configuration. Use the following
command to build the `blinky` firmware.

```
west build -p -b nrf9160dk/nrf9160/ns
```

With your board connected to your local machine, run the following command to
flash the firmware.

```
west flash
```

When the device boots, the LED should be initialized to on.

## Connecting to Golioth

In order for your device to connect to Golioth, you'll need to provide it with
credentials. This example is equipped with a [Zephyr settings
shell](https://docs.zephyrproject.org/latest/services/storage/settings/index.html)
support, which allows you to quickly configure [PSK
credentials](https://docs.golioth.io/device-management/authentication/psk-auth)
using the following commands issued over a serial connection to your device.

```
settings set golioth/psk-id <my-psk-id@my-project>
```

```
settings set golioth/psk <my-psk>
```

```
kernel reboot cold
```
