# OpenThread Border Router (OTBR) Setup

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

## Setup

-   Install OTBR:

    ```shell
    $ sudo ./scripts/centi/centi_mattertool -iotbr
    ```

-   As an alternative way, you can run a VSCode task:

    -   **Ctrl+Shift+P** to open command pallete
    -   Search for **Task** and select **Task: Run Task**
    -   Search for **Install OTBR** and select
    -   Pick OT-RCP platform.
    -   **Insert GIF**

-   If the install fails, delete the directory created:
    ```shell
    $ sudo rm -rf ot-br-posix
    ```

## Troubleshooting

Consult these troubleshooting webpages for more information:

-   [Verify services](https://openthread.io/guides/border-router/build#verify-services)
-   [Access Point Troubleshooting](https://openthread.io/guides/border-router/access-point#troubleshooting)

If mDNS device discovery is not working, check if you are receiving mDNS
advertisements, as stated
[here](https://project-chip.github.io/connectedhomeip-doc/tips_and_troubleshooting/discovery_from_a_host_computer.html):

-   Scanning for commissionable devices
    ```shell
    $ avahi-browse -d local _matterc._udp –resolve
    ```
-   Scanning for commissioned sionable devices
    ```shell
    $ avahi-browse -d local _matter._tcp –resolve
    ```
-   If no mDNS advertisements are being received,
    [troubleshoot Avahi](https://project-chip.github.io/connectedhomeip-doc/tips_and_troubleshooting/troubleshooting_avahi.html).
-   Also check if the devices are in the same network as the OTBR.

### Useful Commands

-   Check if DBus is working and you can retrieve the Active Thread Dataset:

    ```shell
      $ sudo dbus-send --system --dest=io.openthread.BorderRouter.wpan0 --print-reply \
          /io/openthread/BorderRouter/wpan0 \
          io.openthread.BorderRouter.GetProperties \
          "array:string:""ActiveDatasetTlvs"
    ```

-   Check if `mdns` service is active
    ```shell
    $ sudo service mdns status
    ```
-   Check if `otbr-agent` service is active
    ```shell
    $ sudo service otbr-agent status
    ```
-   Retrieve current Thread dataset:
    ```shell
    $  sudo ot-ctl dataset active -x
    ```

# Reference Guides

-   [AN1256: Using the Silicon Labs RCP with the OpenThread Border Router](https://www.silabs.com/documents/public/application-notes/an1256-using-sl-rcp-with-openthread-border-router.pdf)
-   [OpenThread Border Router Build and Configuration](https://openthread.io/guides/border-router/build)
