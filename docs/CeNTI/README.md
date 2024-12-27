# CeNTI Matter Guide

## Introduction

This guide aims to help developers jumpstart a Matter application within the
Matter SDK, which includes:

-   development setup
-   tools installation (Matter SDK, OTBR, MQTT)
-   example applications
-   cross-compilation support

This repository could be used to develop end devices as well, however I would
only see this ever needed if you don't want to use either Silicon Labs or Nordic
(our most used MCUs for wireless appplications) tools or if you want to
contribute to the main repository
[connectedhomeip](https://github.com/project-chip/connectedhomeip) from which
this is repository is forked.

Therefore, this repository is mainly useful to develop Matter controller
applications, build [chip-tool](../guides/chip_tool_guide.md) from source or any
other application code present in the [examples](../../examples/) directory.

## Requirements

The requirements listed here are related to the development of a Matter
controller and the OTBR (for brevity, it will be called Matter Hub).

-   [Hardware Requirements](HARDWARE_REQUIREMENTS.md)
-   [Software Requirements](SOFTWARE_REQUIREMENTS.md)

## Development Environment Setup

> [!WARNING]
>
> Don't clone this repository through Github Desktop because it's not possible
> to do a shallow clone yet in Github Desktop. The cloning process will take a
> while due to a lot of submodules present here and will clone everything (all
> submodules in a recursive way including commits history).

The setup involved requires connecting to the remote device and then installing
all the tools necessary for your Matter application.

1. Check [here](SSH_CONNECTION.md) on how to connect through SSH to the remote
   device where you'll be developing in the VSCode IDE.
2. Open VSCode integrated terminal through shortcut **Ctrl+ Shift+ç**
3. Update packages:
    ```shell
    $ sudo apt-get update
    $ sudo apt-get upgrade
    ```
4. Install git:
    ```shell
     $ sudo apt-get install git
    ```
5. Clone this repository:
    ```
     git clone --depth 1 https://github.com/CENTITVC/Matter
    ```

Inside the directory `Matter/scripts/centi` you can find `centi_mattertool.sh`
which is a script intended to automate the process of installing the required
tools and other functionalities such as building and aid in cross-compilation.
Check script's help by running:

```shell
$ sudo ./scripts/centi/centi_mattertool.sh
```

Refer to the [Matter SDK Setup Guide](MATTER_SDK_SETUP.md) in order to bootstrap
and activate your environment.

If you are building a Matter over Thread application and need to setup an OTBR,
refer to the [OTBR Setup Guide](OTBR_SETUP.md).

## Tools Versioning

Tested with the following commit hashes:

-   Matter (connectedhomeip): tag v1.3.0.0 -
    [5bb5c9e](https://github.com/project-chip/connectedhomeip/tree/5bb5c9e23d532cea40476fc0bd1d3008522792ba)
-   Thread
    -   For `silabs` platform:
        -   OpenThread:
            [7074a43e4](https://github.com/openthread/openthread/tree/7074a43e4)
        -   ot-br-posix:
            [42f98b27b](https://github.com/SiliconLabs/ot-br-posix/tree/42f98b27b)
        -   OT-RCP: Gecko SDK 4.4.2 -
            [e359ba4](https://github.com/SiliconLabs/gecko_sdk/tree/v4.4.2)

## SDK Development

If you want to develop using VSCode Tasks, refer to [guide](VSCODE_DEV.md).

> [!NOTE]
>
> For further integration into the Matter SDK build system, check
> [build_examples.py](../../scripts/build/build_examples.py). Otherwise, you can
> simply run some scripts in order to start developing.

### Building

The directory [examples](../../examples/) will be the folder where you'll
develop your Matter applications. This is due to the fact that it's easier to
integrate in the Matter build system, with the script
[gn_build_example.sh](../../scripts/examples/gn_build_example.sh)

As an alternative, you can simply use the script available or use the GN
commands. Either way, you have two options to build. We'll take the example of
the [MQTT](../../examples/MQTT/) sample which demonstrates a MQTT connection
with the
[Paho Mqtt Cpp Client](https://github.com/eclipse/paho.mqtt.cpp/tree/master)
using Matter SDK build tools.

In your current terminal, make sure you activate your environment in the
repository root directory:

```shell
$ source scripts/activate.sh
```

If you want to use the
[gn_build_examples.sh](../../scripts/examples/gn_build_example.sh):

```shell
$ sudo ./scripts/examples/gn_build_example.sh examples/MQTT/linux/ examples/MQTT/linux/out/
```

Or if you want to use GN tools directly to customize the build configuration:

```shell
$ cd examples/MQTT/linux
$ gn gen out && ninja -C out mqtt-app
```

Inside the `out/` directory, you'll find now the `mqtt-app` executable which you
can run.

```shell
$ ./out/mqtt-app
```

Test the above commands to build the [chip-tool](../../examples/chip-tool/).

### Cross Compilation

Cross compiling allows you to develop code for a specific architecture (x86_64
for a mini-PC with Ubuntu) and then compile the same code to another
architecture (aarch64 for a RPi with Ubuntu OS) from your host machine.

Refer to the full [guide](CROSS_COMPILATION.md) on how to cross-compile Matter
applications.

# Reference Guides or Links

-   [Specification](https://csa-iot.org/developer-resource/specifications-download-request/)
    -   [Google Device Data Model](https://developers.home.google.com/matter/primer/device-data-model)
    -   [Silicon Labs Matter Fundamentals](https://docs.silabs.com/matter/latest/matter-fundamentals-data-model/)
    -   [Nordic Matter](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/protocols/matter/index.html)
-   SDK Development
    -   [Matter Docs](../)
-   Multi Fabric
    -   [Silicon Labs Guide](https://community.silabs.com/s/article/Multi-Fabrics-Admin-second-controller?language=en_US)
    -   [Nordic MultiFabric test with commercial ecosystems](https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/matter-testing-nrf-connect-sdk-platform-with-apple-google-and-samsung-ecosystems-218911247#mcetoc_1gkq24ojdf)
-   [Google Supported Devices](https://developers.home.google.com/matter/supported-devices)
-   Matter Build System
    -   [GN](https://gn.googlesource.com/gn/)
    -   [Ninja](https://ninja-build.org/)
-   Useful Github repos:
    -   https://github.com/project-chip/connectedhomeip
    -   https://github.com/SiliconLabs/matter
    -   https://github.com/nrfconnect/sdk-connectedhomeip
