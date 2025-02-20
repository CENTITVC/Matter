# CeNTI Matter Controller & OTBR

This project implements a Matter controller with Paho MQTT client to be used as
a data gateway to support non-Matter services. Currently the MQTT client
supports the following Matter clusters: - WindowCovering - CO2 - Temperature -
Humidity

## CeNTI Matter Controller & OTBR Versioning

-   Matter (connectedhomeip): tag v1.3.0.0 -
    [5bb5c9e](https://github.com/project-chip/connectedhomeip/tree/5bb5c9e23d532cea40476fc0bd1d3008522792ba)
-   Thread
    -   OpenThread:
        [7074a43e4](https://github.com/openthread/openthread/tree/7074a43e4)
    -   ot-br-posix:
        [42f98b27b](https://github.com/SiliconLabs/ot-br-posix/tree/42f98b27b)
    -   OT-RCP: Gecko SDK 4.4.2 -
        [e359ba4](https://github.com/SiliconLabs/gecko_sdk/tree/v4.4.2)

## Host Requirements for Matter SDK development and OT-BR-Posix SDK

-   Ubuntu 22.04 LTS or newer
-   Follow this [tutorial](https://ubuntu.com/core/docs/install-nuc) to install
    Ubuntu.

**Note**: This tutorial was tested with the following setup:

-   ASUS PN50 Mini-PC
-   Raspberry Pi 4B 4GB

## OTBR-Posix and RCP

The OTBR currently supported is following a RCP configuration, with a Silicon
Labs board providing Thread functionality. It is being used the official
repository of the [OT-BR-Posix](https://github.com/openthread/ot-br-posix) to
build the OpenThread Border Router, instead of the Gecko SDK OT-BR that is shown
in the Silicon Labs Guide Section 3.2.1.

This approach was choosen to keep the size of the tools imported to a minimum.
This approach might have to change in the future since Silicon Labs is phasing
out their [Matter repository](https://github.com/SiliconLabs/matter) to use
within the
[Matter GSDK Extension](https://docs.silabs.com/matter/latest/matter-start/)
available through Simplicity Studio. However, Nordic seems to still base their
OT-RCP from official and open-source tools.

-   Hardware:
    -   Silicon Labs
        [BRD2601](https://www.silabs.com/development-tools/wireless/efr32xg24-dev-kit?tab=overview)
    -   Asus PN50 Mini-PC
-   Guides:
    -   [AN1256: Using the Silicon Labs RCP with the OpenThread Border Router](https://www.silabs.com/documents/public/application-notes/an1256-using-sl-rcp-with-openthread-border-router.pdf)
    -   [OpenThread Border Router Build and Configuration](https://openthread.io/guides/border-router/build)
-   Consult these troubleshooting webpages for more information:
    -   [Verify services](https://openthread.io/guides/border-router/build#verify-services)
    -   [Access Point Troubleshooting](https://openthread.io/guides/border-router/access-point#troubleshooting)

### OT-RCP

Using Simplicity Studio, build and flash the following projects (no changes
needed) to the BRD2601/BRD2703 or any other board that uses the EFR32MG24 MCU
family:

-   bootloader-storage-internal-single 1536k
-   OT-RCP

## SSH connection

-   You can use Visual Studio Code in your Windows machine to connect to the
    Ubuntu machine. Follow this
    [guide](https://code.visualstudio.com/docs/remote/ssh).
-   If you want to access directly through Powershell, run:
    ```shell
        ssh <your_ubuntu_username>@<ubuntu_ip_address>
        ... Insert Password
    ```

## Environment configuration

The approach of the environment configuration relies on a script developed to
help the install of various components needed to compile this project. This
approach was choosen due to its simplicity and to rely on the Matter SDK build
system.

On top of the script, the current code saves a list of Matter devices connected
to their Matter network in the directory `var/matter`, which you should create
before running this controller.

## Support for other Matter device types

### Adding Matter Devices

1.  `MatterDeviceTypes.h`: Create a new define for the new device type ID.
2.  Create your `<MatterDeviceName>.h`. See the other devices for examples and
    Matter Specification to know which data type the new device contains.
3.  Open `MatterDevicesCommon.h` and include the new `<MatterDeviceName>.h`.
    This header is supposed to ease the process of including MatterDevice
    classes.
4.  According to the type of data that the new device contains, create (if
    needed) a new ClusterClient inside `MatterClient/ClusterClient/`. See other
    examples for reference.
5.  Open `MatterClientFactory.cpp` and add the new Clusters.
6.  Open `MatterEndpoint.cpp::AddDeviceType()` write a new case for the new
    device type ID.
7.  Add the code for the new device in the
    `MatterCommands/MatterCommandDeviceSubscription.cpp`.
8.  Inside `IllianceDevices/MatterDevicesCallbacks.cpp` create handlers for the
    new MatterDevice.
9.  Open `AppTask.cpp` and add the new device handlers that inherit the
    MatterDeviceDelegate. Add the device handlers as a AppTask member class in
    `AppTask.h`.

### Adding MQTT commands

1. Inside the `CeNTI_MQTT/` folder sadd the necessary command classes, topic
   parsing and arguments needed for your application.
    - `centi_mqtt_commands.h` are commands received from the MQTT that need to
      interact with a Matter device. Check other commands to see how it is
      implemented.
    - `centi_mqtt.cpp` have the topic parsing and other necessary routines to
      publish measurements or any responses to commands. Check other commmands
      to see how they are implemented.
2. Add to AppTask the necessary routines to include these new commands.
3. Create the command class logic in `application/MatterCommands/` folder.

## TO-DO

1. Refactor code and use more namespaces.
2. Try to make use more of templates to especially for MatterCommands.

## Useful commands

Check messages in Linux DBus with OTBR:

```
sudo dbus-send --system --dest=io.openthread.BorderRouter.wpan0 --print-reply \
        /io/openthread/BorderRouter/wpan0 \
        io.openthread.BorderRouter.GetProperties \
        "array:string:""ActiveDatasetTlvs"
```

Evaluate Linux Service status:

```
sudo service mdns status
sudo service otbr-agent status
```

Read OT dataset:

```
    sudo ot-ctl dataset active -x
```

Generate certificates to other formats:

```
openssl x509 -in centi_broker_CA.crt -out centi_broker_CA.pem -outform PEM
```

Check logs of the Matter controller service:

```
sudo journalctl -f -u centi-matter-controller.service
```

Check if RCP is working:

```
sudo journalctl -f -u otbr-agent.service
```
