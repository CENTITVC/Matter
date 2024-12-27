# Hardware Requirements

There are two options to run the Matter Hub, however for code development and
testing it is recommended to use a PC instead of the Raspberry Pi since the
Matter SDK takes a really long time to build on the RPi and usually fails due to
RAM constraints.

The PC must have the Linux OS (or macOS), since Windows is not very well
supported currently in the Matter SDK. If you want to use Windows, you have to
use WSL and you can only use it to build code, because you won't be able to run
the executable. On a final, you can check this recent [PR] to test if with
DevContainers extension it's possible to run Matter inside Windows.

Nonetheless, ESP has a
[guide](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html)
where Windows is supported with some changes in the BIOS settings.

-   **PC**

    -   Used the Asus PN50 Mini-PC
    -   Ubuntu 22.04 (20.04 or newer)
    -   Follow this [tutorial](https://ubuntu.com/core/docs/install-nuc) to
        install Ubuntu (you can use the Server version if you don't need the
        GUI).

-   **Raspberry Pi 4**
    -   Tested with the RPi 4B 4GB
    -   SD card with storage of 64GB
    -   Ubuntu 22.04 (20.04 or newer) not Raspbian OS
    -   I have installed the Ubuntu Server 22.04
    -   Follow this
        [tutorial](https://ubuntu.com/tutorials/how-to-install-ubuntu-on-your-raspberry-pi#1-overview).

## Matter Over Thread Requirements

The currently tested setup involves running Matter over Thread in a Radio
Co-Processor (RCP) design. The RCP board provides Thread functionality to the
host device (PC/RPi) since neither can provide Thread radio.

At the moment, the Matter over Thread was only tested with Silicon Labs boards
as RCP.

### Silicon Labs RCP

-   **Boards**
    -   [BRD2601](https://www.silabs.com/development-tools/wireless/efr32xg24-dev-kit?tab=overview)
        -   `bootloader-storage-internal-single 1536k`
        -   `OT-RCP`

#### Build and Flash

SDKs tested:

-   Gecko SDK 4.4.0
-   Gecko SDK 4.4.2

Using Simplicity Studio, build and flash the projects (no changes needed) to the
BRD2601 or any other board that uses the EFR32MG24 MCU family.
