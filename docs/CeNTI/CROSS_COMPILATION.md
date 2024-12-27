# Cross Compilation

This guide was tested against a Raspberry Pi 4B 4GB as a Matter Hub.

## Setup

### RPi

1. Do a fresh install of Ubuntu Server 64bit. You can follow the notes in the
   [hardware requirements](HARDWARE_REQUIREMENTS.md).
2. Install the requirements specific to the Raspberry Pi 4
   [here](https://project-chip.github.io/connectedhomeip-doc/guides/BUILDING.html#installing-prerequisites-on-raspberry-pi-4).
3. Install some essentials packages for building:
    ```
    sudo apt-get install build-essential gcc
    ```
4. Transfer `centi_mattertool.sh` to the Raspberry Pi 4:
    ```shell
    $ scp scripts/centi/centi_mattertool.sh <hostname@IP>:/<directory_path_rpi>
    ```
5. Install OTBR:
    ```shell
    $ centi_mattertool.sh -iotbr
    ```

### Host Setup

1. Install the appropriate toolchain for the cross compilation target, which in
   this case is the RPi:
    ```shell
    $ sudo apt install g++-aarch64-linux-gnu
    ```
2. Create a directory in order to store the RPi Sysroot:
    ```shell
    $ sudo mkdir /opt/rpi-sysroot
    ```

## Building for target

For building the target, I am using the developed [VSCode tasks](VSCODE_DEV.md)
since it's easier than writing all the commands. You can check the
[centi_mattertool.sh](../../scripts/centi/centi_mattertool.sh) to see which
commands are run.

1. Transfer the system libraries from the Raspberry Pi in order to create the
   RPi Sysroot:
    - **Ctrl+Shift+P** to trigger command pallete
    - Select **Tasks: Run Task**
    - Type **Create RPi Sysroot** and select
    - Enter RPi info to connect
2. Build the application for the target architecture:
    - Run VSCode Task **Build RPi Sysroot**
    - Select the application to build
    - The executable will be place inside the
      `<application directory>/out/aarch64`
3. At this step, you can now transfer the binary to the Raspberry Pi:
    - Run VSCode Task **Transfer binary to RPi**
    - Select binary
    - Enter RPi info to connect
    - Write the full path of the directory to store the binary

## Running

In the directory you have stored the binary, you can simply run:

```shell
$ ./<path_to_binary>
```

# Reference Guide

-   [CHIP Build Cross-Compile](../../integrations/docker/images/stage-1/chip-build-crosscompile/README.md).
