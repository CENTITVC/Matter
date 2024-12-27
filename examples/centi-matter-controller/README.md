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
needed) to the BRD2601 or any other board that uses the EFR32MG24 MCU family:

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

However, a more suitable approach to use within git would be to have different
gitmodules to pull the repositores and checkout the correct submodules to the
desired version. This could also make the commits from this repository easier.
Different things could be done to use this approach such as:

-   .vscode folder (with settings, tasks, IntelliSense configuration) could be
    created in order to correct the path to the Matter SDK (since it's not
    anymore on git root folder), have more VSCode tasks oriented to our work,
    include CeNTI Library and whatsoever;
-   custom build script (gn_build_centi_examples.sh - similar to
    gn_build_example.sh) to only consider the CeNTI projects;

The problem I see with this approach is the fact that the Matter SDK already has
a very strong VSCode development know-how and the environment is already setup
to use with this IDE.

For instance, VSCode doesn't support yet mono repositories with different
.vscode folders (it only recognizes the .vscode folder from repo's root ). This
could be avoided with a
[multi-root workspace configuration](https://code.visualstudio.com/docs/editor/multi-root-workspaces)
but it's not something I'd personally like.

On another note, the custom build script could be broken in future Matter SDK
releases if they decide to change the build system.

Of course this is possible to be done, but it's something it will be needed to
be worked on future releases of this repository. In the future, I could see the
possibility of releasing a DevContainer or a Docker container to improve this
setup. For now, we have these steps :)

-   Clone this repository:
    ```shell
        git clone https://github.com/CENTITVC/ILCE-iNERGY
    ```
-   Inside ILCE-iNergy/scripts you can find centi_mattertool.sh, which is a
    script intended to automate the process of installing Paho MQTT libraries,
    bootstrap Matter SDK and OTBR, and other functions. See script's help by
    running:
    ```
        sudo ./ILCE-iNERGY/Firmware/scripts/centi_mattertool.sh
    ```
-   In **$HOME** directory (/home/<ubuntu_username>), run:

    ```shell
        sudo ./scripts/centi_mattertool.sh -iotbr
        sudo ./scripts/centi_mattertool.sh -imqtt
        sudo ./scripts/centi_mattertool.sh -imatter
        sudo apt install g++-aarch64-linux-gnu
    ```

-   Create directory to store CHIP persistent data:
    ```
        sudo mkdir /var/matter
    ```

## Cross Compilation for Raspberry Pi

1.  In a fresh install of Ubuntu Server 64bit, install the requirements noted
    [here](https://project-chip.github.io/connectedhomeip-doc/guides/BUILDING.html#installing-prerequisites-on-raspberry-pi-4).
2.  Install some essentials packages for building:
    ```
    sudo apt-get install build-essential gcc libssl-dev cmake
    ```
3.  Install Paho MQTT C++ library:

        git clone https://github.com/eclipse/paho.mqtt.cpp
        cd paho.mqtt.cpp
        git checkout v1.4.1
        git submodule init
        git submodule update
        sudo cmake -Bbuild -H. -DPAHO_WITH_MQTT_C=ON -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=OFF -DPAHO_BUILD_SAMPLES=OFF
        sudo cmake --build build/ --target install
        sudo ldconfig

4.  Install OTBR using `centi_mattertool.sh`.

5.  Create the sysroot and transfer it to host machine (here it is my mini-PC).
    ```
    sudo mkdir /opt/rpi-sysroot
    export PI="192.168.62.225"
    sudo rsync -avL ubuntu@$PI:/lib /opt/rpi-sysroot
    sudo rsync -avL ubuntu@$PI:/usr/lib /opt/rpi-sysroot/usr
    sudo rsync -avL ubuntu@$PI:/usr/include /opt/rpi-sysroot/usr
    # for paho mqtt libraries
    sudo rsync -avL ubuntu@$PI:/usr/local /opt/rpi-sysroot/usr
    ```

> [!NOTE] If ssh-copy-id fails ubuntu@PI run `ssh-keygen` to generate a new pair
> of public keys. Usually it is only needed for the first time.

> [!TIP] You can use `centi_mattertool.sh` with `-brpi` flag.

5.  Build for Raspberry Pi (aarch64)

    -   In `centi-matter-controller.gni`: `config_cross_compilation = true`
    -   Run the following commands. Change `--root` flag and executable
        destination path according to your needs:

        ```
        source scripts/activate.sh
        PKG_CONFIG_PATH="/opt/rpi-sysroot/lib/aarch64-linux-gnu/pkgconfig" \
        gn gen --check --fail-on-unused-args --export-compile-commands --root=examples/centi-matter-controller '"'"'--args=is_clang=false treat_warnings_as_errors=false target_cpu="arm64" sysroot="/opt/rpi-sysroot"'"'"' out/centi-matter-controller-rpi'
        ninja -v -C 'out/centi-matter-controller-rpi'
        ```

    > [!TIP] Use `centi_mattertool.sh` to run the commands.

    > [!WARNING] If the output of the cross-compilation warns about
    > `aarch64-linux-gnu-gcc not found` run the command to install your
    > cross-compiler toolchain: `sudo apt install g++-aarch64-linux-gnu`

6.  Transfer the executable to the Raspberry Pi. In my case, I also transfer the
    MQTT server certificate and the config.json file.

    ```
        scp out/centi-matter-controller-rpi/centi-matter-controller ubuntu@192.168.62.225:/home/ubuntu/matter-controller
        scp examples/centi-matter-controller/CeNTI_MQTT/centi_broker_CA.pem ubuntu@192.168.62.225:/home/ubuntu/matter-controller
        scp examples/centi-matter-controller/config.json ubuntu@192.168.62.225:/home/ubuntu/matter-controller
    ```

7.  (Optional) Create a Linux system service in order to run your Matter
    controller automatically on reboot. In order to do this, make sure:

    ```
         sudo nano /etc/systemd/system/centi-matter-controller.service
    ```

    Copy this text (change ExecStart path to where it is stored the executable):

    ```
        [Unit]
        Description=CeNTI Matter Controller
        After=otbr-agent.service

        [Service]
        ExecStart=/home/smart_systems/connectedhomeip/out/centi-matter-controller/centi-matter-controller

        Restart=always
        RestartSec=3
        User=root
        SyslogIdentifier=centi-matter-controller

        [Install]
        WantedBy=multi-user.target
    ```

    Update daemon and restart service:

    ```
        sudo systemctl daemon-reload
        sudo systemctl restart centi-matter-controller.service
    ```

    Check if the service is active and running:

    ```
        sudo systemctl status centi-matter-controller.service
    ```

8.  (Optional) Disable sleep mode in order to continuously run the Matter
    Controller:

    ```
    sudo systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target
    ```

9.  (Optional) Change storage directory directly in Matter SDK since `/tmp`
    directory can be deleted after reboot:
    -   Under `connectedhomeip/src/platform/Linux/CHIPPlatformConfig.h` set
        CHIP_CONFIG_KVS_PATH:
        `#define CHIP_CONFIG_KVS_PATH "/var/matter/chip_kvs"`
    -   **_Review this later since gn flags set in
        centi-matter-controller.gni+BUILD.gn or define this in out application
        folder `include/CHIPProjectAppConfig.h` should work but it isn't._**

## Support for other Matter device types

1.  `MatterDeviceTypes.h`: Create a new define for the new device type ID.
2.  Create your `<MatterDeviceName>.h`. See the other devices for examples and
    Matter Specification to know which data type the new device contains.
3.  Open `MatterDevicesCommon.h` and include the new `<MatterDeviceName>.h`.
    This header is supposed to ease the process of including MatterDevice
    classes.
4.  According to the type of data that the new device contains, create (if
    needed) a new ClusterClient inside `MatterClient/ClusterClient/`. See other
    examples for reference. **Currently, it's only supported the read,
    subscription (attributes and events) and commands operations**.
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

### Support with MQTT commands

1. Following the steps mentioned before, you must add inside the `CeNTI_MQTT/`
   folder the necessary command classes, topic parsing and arguments needed for
   your application. Check other commands to see how it is implemented (better
   way to implement this would be using templates, but I didn't have the time to
   figure out the best way to implement that for these commands since they use
   variable arguments and we want to use C++11 standard like Matter SDK).
2. Add to AppTask the necessary routines to include these new command.
3. Create the command class logic in `application/MatterCommands/` folder
   (again, like in chiptool, better way would be to generalize all commands,
   however didn't think the string commands approach would be the best due to
   being error prone and harder to make it modular).

## TO-DO

1. Refactor code and use more namespaces.
2. Try to make use more of templates to especially for MatterCommands.

## Useful commands

```
sudo dbus-send --system --dest=io.openthread.BorderRouter.wpan0 --print-reply \
        /io/openthread/BorderRouter/wpan0 \
        io.openthread.BorderRouter.GetProperties \
        "array:string:""ActiveDatasetTlvs"

sudo service mdns status
sudo service otbr-agent status
sudo ot-ctl dataset active -x
openssl x509 -in centi_broker_CA.crt -out centi_broker_CA.pem -outform PEM
```
