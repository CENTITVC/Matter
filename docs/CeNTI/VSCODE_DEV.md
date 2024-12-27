# Visual Studio Code Development

## Building Tasks

-   **[CeNTI] Build CeNTI Matter Application**
    -   Build applications stored in [examples](../../examples/) with `centi`
        prefix.
    -   Build output is stored inside `examples/<appDir>/out`
-   **[CeNTI] Build Matter Application**
    -   Build any Matter application stored in [examples](../../examples/)
    -   Build output is stored inside `examples/<appDir>/out`

## Cross-Compilation Tasks

-   **[CeNTI] Create RPi Sysroot**
    -   Create the system root for the RPi
    -   Sysroot is stored in `/opt/rpi-sysroot`
    -   Make sure the directory exists
    -   Enter RPi address with hostname in the prompt
-   **[CeNTI] Build RPi Sysroot**
    -   Build the application stored inside `examples`
    -   Select the application stored
    -   The executable will be created inside
        `<application directory>/out/aarch64`
-   **[CeNTI] Transfer binary to RPi**
    -   Transfer the executable binary to the Raspberry Pi (can be any other
        remote device though)
    -   Select from the list of executables found inside `examples` (maximum
        depth to find it is 5 levels)
    -   Enter `hostname@IP` to connect to the RPi
    -   Write the full path of the directory to write it

## Debugging

## Setup

> [!NOTE]
>
> If you are not connected over SSH to the remote host you may skip this step.

Since VSCode Remote-SSH Extension does not support shell commands with root
permissions (e.g. with sudo) a workaround was needed to make this work:

-   [scripts/sudo_gdb.sh](../../scripts/centi/sudo_gdb.sh)
-   Elevating the permissions of GCC:

    -   Create the file
        `/usr/share/polkit-1/actions/freedesktop.policykit.pkexec.run-gdb.policy`
        with the content:

        ```xml
        <?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE policyconfig PUBLIC
        "-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN"
        "http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd">
        <policyconfig>

        <action id="org.freedesktop.policykit.pkexec.run-gdb">
            <description>Some Description</description>
            <message>Some Message</message>
            <defaults>
            <allow_any>no</allow_any>
            <allow_inactive>yes</allow_inactive>
            <allow_active>yes</allow_active>
            </defaults>
            <annotate key="org.freedesktop.policykit.exec.path">/usr/bin/gdb</annotate>
        </action>

        </policyconfig>
        ```

### Run VSCode debugger

In order to debug an application inside VSCode Native Debug:

-   Go to Run and Debug (Ctrl+Shift+D)
-   Select one of the configurations
-   Start debugging (play button / F5)

### Available launch configurations

-   **[CeNTI] Run CeNTI Matter Application**
    -   Debugs any Matter application that has an executable stored inside
        `examples` with prefix `centi`.
-   **[CeNTI] Run Matter Application**
    -   Debug any Matter application inside `examples`.

## Helpers

-   **[CeNTI] Install OTBR**
    -   Installs the OTBR and checks out the relevant commits according to the
        OT-platform you're running in (silabs/nrf)
