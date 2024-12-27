#!/usr/bin/env python3
import sys
from datetime import datetime
import os

VERSION_INFO_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__),
                '../../third_party/paho.mqtt.cpp/repo/externals/paho-mqtt-c/'))
VERSION_INFO_PATH = os.path.abspath(os.path.join(VERSION_INFO_DIR, 'src/VersionInfo.h.in'))
OUT_VERSION_INFO_PATH = os.path.abspath(os.path.join(VERSION_INFO_DIR, 'src/VersionInfo.h'))

def read_version_file(version_file):
    """Read the content of the version file and return it as a string."""
    with open(version_file, 'r') as f:
        return f.read().strip()

def replace_placeholders(input_file, output_file, version_dir):
    # Read the version from the paho-mqtt-c repo
    major = read_version_file(os.path.join(version_dir, "version.major"))
    minor = read_version_file(os.path.join(version_dir, "version.minor"))
    patch = read_version_file(os.path.join(version_dir, "version.patch"))

    client_version = f"{major}.{minor}.{patch}"

    with open(input_file, 'r') as infile:
        content = infile.read()

    # Generate the current timestamp
    build_timestamp = datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC")

    # Replace placeholders
    content = content.replace("@BUILD_TIMESTAMP@", build_timestamp)
    content = content.replace("@CLIENT_VERSION@", client_version)

    # Write the result to the output file
    with open(output_file, 'w') as outfile:
        outfile.write(content)

if __name__ == "__main__":
    replace_placeholders(VERSION_INFO_PATH, OUT_VERSION_INFO_PATH, VERSION_INFO_DIR)
