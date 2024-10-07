#!/bin/bash

if [ $(id -u) -ne 0 ]; then
    echo "Please run as root"
    exit 0
fi

SCRIPT_DIR=basedir=$(dirname "$0")
BASE_DIR=$(cd "${basedir}"; pwd)
USER_HOME=$(getent passwd $SUDO_USER | cut -d: -f6)
THIRD_PARTY_DIR="$BASE_DIR/third_party"

MATTER_ROOT="$SCRIPT_DIR/../.."
MQTT_ROOT="$THIRD_PARTY_DIR/paho.mqtt.cpp"
OTBR_ROOT="$MATTER_ROOT/third_party/ot-br-posix/repo"
OT_ROOT="$MATTER_ROOT/third_party/openthread/repo"

check_repo_exists() {
    repoUrl="$1"
    retval="$(git ls-remote -q "$repoUrl" &>/dev/null)"
    if [[ "$?" -eq 0 ]]; then
        return 0
    else
        return 1
    fi
}

echo_green() {
    echo -e "\033[0;32m$*\033[0m"
}

echo_blue() {
    echo -e "\033[1;34m$*\033[0m"
}

echo_bold_white() {
    echo -e "\033[1;37m$*\033[0m"
}

echo_red() {
    echo -e "\033[1;31m$*\033[0m"
}

echo_banner() {
    echo "   ___       _  _  _____  ___   __  __        _    _               "
    echo "  / __| ___ | \| ||_   _||_ _| |  \/  | __ _ | |_ | |_  ___  _ _   "
    echo ' | (__ / -_)| .` |  | |   | |  | |\/| |/ _` ||  _||  _|/ -_)| `_|  '
    echo "  \___|\___||_|\_|  |_|  |___| |_|  |_|\__,_| \__| \__|\___||_|    "
    echo ""
    echo ""
    echo ""
}

Print_Help() {
    echo "This bash script simplifies the process of installing and building Matter SDK, OpenThread Border Router and CeNTI Matter Controller."
    echo "Here is an overview of the available commands :"
    for commands in "${!cmd_list[@]}"; do echo $commands; done

    echo_bold_white "Available options  :"
    echo " -h, --help			                                            Print this help."
    echo " -imqtt, --install_mqtt 		                                    Install Paho MQTT client Libraries."
    echo " -imatter, --install_matter_sdk 		                            Install Matter SDK."
    echo " -iotbr, --install_otbr                                           Install OTBR POSIX."
    echo " -sthread, --start_otbr                                           Create and start new thread network."
    echo " -crpisys <hostname@IP>, --create_rpi_sysroot <hostname@IP>       Create the RPi sysroot on your host machine."
    echo " -brpi, --build_rpi                                               Cross-Compile build for Rasberry Pi 4 (64bit Ubuntu Server)."
}

Install_MQTT() {
    echo_blue "Starting Paho MQTT installation..."

    # Update package lists and install dependencies
    echo_blue "Installing required packages..."

    apt-get update
    apt-get install build-essential gdb libssl-dev cmake git doxygen

    cd $THIRD_PARTY_DIR

    if [[ -d "$MQTT_ROOT" ]]; then
        echo_blue "paho.mqtt.cpp already exists"
    else
        git clone https://github.com/eclipse/paho.mqtt.cpp.git
        cd paho.mqtt.cpp
        git checkout v1.4.1
        git submodule init
        git submodule update
        sudo cmake -Bbuild -H. -DPAHO_WITH_MQTT_C=ON -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=OFF -DPAHO_BUILD_SAMPLES=OFF
        sudo cmake --build build/ --target install
        ldconfig
        cd $BASE_DIR
    fi
}

Install_Matter_SDK() {
    local MATTER_SDK_VERSION=${1:-"v1.3.0.0"} # Default to v1.3.0.0 if no version is passed

    echo_blue "Starting Matter SDK installation..."
    # Update package lists and install dependencies
    echo_blue "Installing required packages..."

    apt-get update
    apt-get install git gcc g++ pkg-config libssl-dev libdbus-1-dev \
        libglib2.0-dev libavahi-client-dev ninja-build python3-venv python3-dev \
        python3-pip unzip libgirepository1.0-dev libcairo2-dev libreadline-dev || {
        echo_red "Failed to install dependencies."
        exit 1
    }

    cd $MATTER_ROOT

    echo_blue "Updating submodules..."
    ./scripts/checkout_submodules.py --shallow --platform linux

    echo_blue "Activating Matter environment"
    source scripts/bootstrap.sh

    echo_green "Matter SDK installed!"
}

Create_RPISysroot() {
    if [ -z "$1" ]; then
        echo "Error: No hostname@IP provided."
        exit 1
    fi

    local PI="$1" # Use the passed IP address as the PI variable

    cd $MATTER_ROOT || {
        echo "Failed to change directory to $MATTER_ROOT"
        exit 1
    }
    sudo rsync -avL $PI:/lib /opt/rpi-sysroot
    sudo rsync -avL $PI:/usr/lib /opt/rpi-sysroot/usr
    sudo rsync -avL $PI:/usr/include /opt/rpi-sysroot/usr
    # for paho mqtt libraries
    sudo rsync -avL $PI:/usr/local /opt/rpi-sysroot/usr
}

Build_RPI() {
    source $MATTER_ROOT/scripts/activate.sh
    bash -c '
    PKG_CONFIG_PATH="/opt/rpi-sysroot/lib/aarch64-linux-gnu/pkgconfig" \
    gn gen --check --fail-on-unused-args --export-compile-commands --root=examples/centi-matter-controller '"'"'--args=is_clang=false treat_warnings_as_errors=false target_cpu="arm64" sysroot="/opt/rpi-sysroot"'"'"' out/centi-matter-controller-rpi'
    ninja -C 'out/centi-matter-controller-rpi'
}

Install_OTBR() {
    # Currently using Silicon Labs OT-RCP project so trying here to make the OTBR build compatible
    # Commit hashes are below
    # https://github.com/SiliconLabs/matter/releases/tag/v2.3.0-1.3
    otbrPosixCommit="42f98b27b"
    otCommit="7074a43e4"

    ethIfName=$(ip link | awk -F: '$0 !~ "lo|vir|wl|^[^0-9]"{print $2;getline}' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
    
    git config --global --add safe.directory $MATTER_ROOT
    git config --global --add safe.directory $OTBR_ROOT
    git config --global --add safe.directory $OT_ROOT

    cd $OTBR_ROOT
    #git clone https://github.com/openthread/ot-br-posix
    git checkout $otbrPosixCommit
    ./script/bootstrap
    cd $OT_ROOT
    git checkout $otCommit
    cd $OTBR_ROOT
    INFRA_IF_NAME=$ethIfName ./scripts/setup
}

Start_NewThreadNetwork() {
    echo_green “Starting a new thread network”
    sudo ot-ctl factoryreset
    sleep 3
    sudo ot-ctl srp server disable
    sudo ot-ctl thread stop
    sudo ot-ctl ifconfig down
    sudo ot-ctl dataset init new
    sudo ot-ctl dataset commit active
    sudo ot-ctl srp server enable
    sudo ot-ctl ifconfig up
    sudo ot-ctl thread start
    sleep 7
}

Clean_Install_OTBR() {
    echo_blue "Cleaning OTBR directory..."
    rm -rf "$OTBR_ROOT/*"
}

echo_banner

echo_bold_white "BASE_DIR: $BASE_DIR"
echo_bold_white "THIRD_PARTY_DIR: $THIRD_PARTY_DIR"
echo_bold_white "CHIP DIR: $MATTER_ROOT"
echo_bold_white "MQTT_DIR: $MQTT_ROOT"
echo_bold_white "OTBR_DIR: $OTBR_ROOT"
echo_bold_white "OT_ROOT: $OT_ROOT"

if [ $# -eq 0 ]; then
    Print_Help
    exit 0
fi

while [ $# -gt 0 ]; do
    case $1 in
    --help | -h)
        Print_Help
        exit 0
        ;;
    --install_mqtt | -imqtt)
        Install_MQTT
        exit 0
        ;;
    --install_matter_sdk | -imatter)
        Install_Matter_SDK
        exit 0
        ;;
    --create_rpi_sysroot | -crpisys)
        shift
        if [ -n "$1" ]; then       # Check if an argument (IP) is provided
            Create_RPISysroot "$1" # Pass the IP to the function
            exit 0
        else
            echo "Error: --create_rpi_sysroot requires an IP address."
            exit 1
        fi
        ;;
    --build_rpi | -brpi)
        Build_RPI
        exit 0
        ;;
    --install_otbr | -iotbr)
        Install_OTBR
        exit 0
        ;;
    --start_otbr | -sthread)
        Start_NewThreadNetwork
        exit 0
        ;;
    *)
        #unknown arg
        echo_red "Wrong argument supplied"
        Print_Help
        exit 0
        ;;
    esac
done
