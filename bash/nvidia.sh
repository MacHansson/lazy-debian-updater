#!/bin/bash

# Ensure script is run as root
if [ "$EUID" -ne 0 ]; then
  echo "Error: Please run as root (sudo)."
  exit 1
fi

echo "--- NVIDIA 580-Series Branch Installer ---"

# --- Configuration ---
TARGET_DIR="/home/marc/Downloads"
mkdir -p "$TARGET_DIR"

# 1. Fetch the latest version within the 580 branch from the Master Index
# We use sort -V to ensure that 580.142 is ranked higher than 580.9
echo "Searching for the latest 580.x driver..."
VERSION=$(curl -sL https://download.nvidia.com/XFree86/Linux-x86_64/ | \
    grep -oE '580\.[0-9]+(\.[0-9]+)?' | sort -V | tail -n 1)

if [ -z "$VERSION" ]; then
    echo "Error: Could not find any version starting with '580' in the NVIDIA index."
    exit 1
fi

echo "Found latest 580 branch version: $VERSION"

FILENAME="NVIDIA-Linux-x86_64-${VERSION}.run"
FULL_PATH="$TARGET_DIR/$FILENAME"
URL="https://us.download.nvidia.com/XFree86/Linux-x86_64/${VERSION}/${FILENAME}"

# 2. Download
if [ ! -f "$FULL_PATH" ]; then
    echo "Downloading: $URL"
    curl -L -o "$FULL_PATH" "$URL"
    
    if [ ! -s "$FULL_PATH" ]; then
        echo "Error: Download failed or file is empty."
        rm "$FULL_PATH"
        exit 1
    fi
else
    echo "File already exists at: $FULL_PATH"
fi

chmod +x "$FULL_PATH"

# 3. Execute Installation
echo "Starting installation (Silent mode, GUI-safe)..."

"$FULL_PATH" \
  --silent \
  --dkms \
  --allow-installation-with-running-driver \
  --no-nouveau-check \
  --rebuild-initramfs \
  --run-nvidia-xconfig \
  --accept-license

if [ $? -eq 0 ]; then
    echo "------------------------------------------------------------"
    echo "SUCCESS: NVIDIA $VERSION is installed."
    echo "REBOOT your system to switch to the new driver."
    echo "------------------------------------------------------------"
else
    echo "ERROR: Installation failed. Check /var/log/nvidia-installer.log"
fi
