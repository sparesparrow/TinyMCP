# tinymcp

Let LLMs control embedded devices via the [Model Context
Protocol](https://modelcontextprotocol.io/).

> [!WARNING]
> `tinymcp` is an experimental project. Expect frequent breaking changes.

## Background

The `tinymcp` project includes an MCP server, which is executed locally, but
communicates with remote devices via the [Golioth management
API](https://docs.golioth.io/reference/management-api/). It uses two existing
device services on the Golioth platform, [LightDB
State](https://docs.golioth.io/application-services/lightdb-state/) and [Remote
Procedure Calls (RPCs)](https://docs.golioth.io/device-management/rpc/), to
expose device functionality via MCP.

RPCs can be exposed as
[tools](https://modelcontextprotocol.io/specification/2025-06-18/server/tools)
by including a schema in on the `mcp/tools/schema` path in LightDB State.
Because LightDB State can be updated by devices or users, RPCs on existing
devices can be exposed without needing to modify the device firmware.

## Setup

To use `tinymcp`, you need to connect a device to the Golioth platform, then run
the `tinymcp` server on your local machine.

### Firmware

While existing devices can be used without changing the firmware, if you don't
already have a device on Golioth, the [blinky example](./examples/blinky)
provides a simple demonstration of how to expose LED control to an LLM via
`tinymcp`. It uses [Zephyr RTOS](https://www.zephyrproject.org/), which requires
[setting up
dependencies](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
before building. After doing so, you can initialize your workspace with one of
the following commands.

```
west init -l .
```

Or, to use the downstream Nordic [nRF Connect SDK](https://github.com/nrfconnect/sdk-nrf).

```
west init -l . --mf west-ncs.yml
```

With your workspace initialized, use `west update` to fetch necessary modules
and libraries.

```
west update --narrow -o=--depth=1
```

You are now ready to build the firmware and flash your device. See the [blinky
documentation](./examples/blinky/README.md) for more information.

### Server

In order to run the `tinymcp` MCP server locally, the following environment
variables need to be supplied.

```
TINYMCP_PROJECT=<your-golioth-project-id>
TINYMCP_DEVICE=<your-golioth-device-id>
TINYMCP_API_KEY=<api-key-for-your-golioth-project>
```

The server can be built into an executable using `go build -o tinymcp ./server`
or run directly with `go run ./server`.

If you don't already have a device on Golioth, the [blinky
example](./examples/blinky) demonstrates how to expose LED control to an LLM via
`tinymcp`. See the [documentation](./examples/blinky/README.md) to build and
flash on your device, then expose the registered RPCs in your MCP schema.

There are many [MCP client
implementations](https://modelcontextprotocol.io/clients), most of which should
be compatible with `tinymcp`. Setup for a few popular options are listed below.

#### MCP Inspector

[MCP Inspector](https://github.com/modelcontextprotocol/inspector) is a visual
testing tool for MCP servers. It allows you to interact with MCP methods
directly, rather than going through an LLM. This can be useful when
troubleshooting why an LLM may be failing to interact with your device.

The
[documentation](https://github.com/modelcontextprotocol/inspector/blob/main/README.md)
provides information on how to quickly start the inspector. Once it is running,
you can point it to your local `tinymcp` MCP server by entering
`http://localhost:8080` as the address and clicking `Connect`. You should be
presented with an option to `List Tools`, which should result in all of the RPCs
in your schema being exposed.

#### Claude Code

[Claude Code](https://docs.anthropic.com/en/docs/claude-code/overview) is an
agentic coding tool that you interact with in the terminal.

With the `tinymcp` MCP server running locally, it can be [added to Claude
Code](https://docs.anthropic.com/en/docs/claude-code/mcp) with a single command.

```
claude mcp add --transport http tinymcp http://localhost:8080 -H 'Authorization: Bearer none'
```

> [!NOTE]
> The `Authorization` header is supplied in this case to indicate to Claude Code
> that it should not attempt to authenticate via the OAuth flow.

#### Gemini CLI

The [Gemini CLI](https://github.com/google-gemini/gemini-cli) is an open-source
AI agent that allows you to access [Gemini](https://gemini.google.com) from the
terminal.

The [`.gemini/settings.json`](./.gemini/settings.json) file in this repository
will automatically configure the CLI to connect to a locally running `tinymcp`
MCP server if the CLI is started in this repository.
