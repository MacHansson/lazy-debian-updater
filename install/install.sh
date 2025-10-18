#!/bin/bash

# Check for sudo/root privileges
if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root. Try using sudo."
  exit 1
fi

# Continue with the rest of the script...
