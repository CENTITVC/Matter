# SSH Connection

### Step 1: IP Address of the Matter Hub

1. If you don't know the IP connect the Hub to a monitor through an HDMI cable
   and connect a keyboard.
2. Run in the command line:
    ```shell
    $ ip a
    ```

### Step 2: SSH Connection

Knowing the IP, it's now possible to establish a SSH connection. Check this
[guide](https://code.visualstudio.com/docs/remote/ssh) for further detail.

> [!TIP]
>
> If you don't know the hostname you have set before, run in the command line
> `hostname` to get it.

1. Ensure that the Matter Hub and your laptop are in the same network, either
   via Ethernet or Wi-Fi.
2. In VSCode:
    - You can follow this
    - **Ctrl+Shift+P** to trigger command pallete
    - Search for Connect to Host and select `Remote-SSH: Connect to Host`
    - Enter in the prompt the hostname@IP of your Matter Hub
3. If you want to access directly through Powershell, run:
    ```shell
    $ ssh <hostname>@<ip>
    ... Insert Password
    ```
