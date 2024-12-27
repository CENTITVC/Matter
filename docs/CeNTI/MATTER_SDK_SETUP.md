# Matter SDK Setup

## 1: Initial Setup

After establishing the SSH connection inside VSCode make sure the workspace
folder is this repository, otherwise open the folder (**Ctrl+O**), which will
prompt you to connect again over SSH.

Usually after this, if you press **Ctrl+Shift+N** to open a New Window VSCode
will save the configuration and you can instantly connect over SSH and inside
this workspace.

Additionally, make sure you install the recommended extensions for this
repository.

## Step 2: Activating the environment

-   Install the dependencies specific to Linux OS:

```shell
$ sudo apt-get install git gcc g++ pkg-config libssl-dev libdbus-1-dev \
     libglib2.0-dev libavahi-client-dev ninja-build python3-venv python3-dev \
     python3-pip unzip libgirepository1.0-dev libcairo2-dev libreadline-dev \
     default-jre
```

-   Move to the directory:
    ```shell
    $ cd Matter
    ```
-   If needed, create a branch. Usually, it's best practice to ensure the
    repository version is compatible with the OTBR tool. In this case, since I'm
    using Silicon Labs based OTBR configuration, I am using version 1.3.0.0
    (tag) from connectedhomeip.
    ```shell
    git checkout -b <branch_name> v1.3.0.0
    ```
-   Only checkout the relevant submodules for your platform:
    ```shell
        ./scripts/checkout_submodules.py --shallow --platform linux
    ```
-   Bootstrap your environment. You only need to do this once since it's time
    expensive or if some packages go out of date:
    ```shell
      $ source scripts/bootstrap.sh
    ```
-   If you need to active the environment again in your shell run:
    ```shell
      $ source scripts/activate.sh
    ```

## Troubleshooting

If bootstrap fails:

-   Check if all dependencies are installed
-   Check if the right OS is installed:
    ```shell
    $ uname -a
    ```
-   Python3 env or Pigweed errors:
    -   Reboot
    -   Bootstrap again
    -   If this fails, delete the repository and git clone it again.

## Reference Guide

-   [Building CHIP](../guides/BUILDING.md)
