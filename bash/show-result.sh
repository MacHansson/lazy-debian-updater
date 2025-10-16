#!/usr/bin/env bash

source "$(dirname "$(realpath "$0")")/config"
os=$(grep -oP '^PRETTY_NAME="\K[^"]+' /etc/os-release)
MESSAGE=""
SUCCESS=1
UPDATED=0

if [ "$1" -eq 0 ]; then
    MESSAGE+="$os is up to date."
elif [ "$1" -eq 1 ]; then
    MESSAGE+="An apt upgrade was done."
    UPDATED+=1
elif [ "$1" -eq 2 ]; then
    MESSAGE+="WARNING - Some packages were kept back."
    SUCCESS=0
else
    MESSAGE+="WARNING - The apt upgrade failed to complete."
    SUCCESS=0
fi

if [ "$2" -eq 0 ]; then
    MESSAGE+="<br>All flatpaks are up to date."
elif [ "$2" -eq 1 ]; then
    MESSAGE+="<br>A flatpak update was done."
    UPDATED+=1
else
    MESSAGE+="<br>WARNING - The flatpak update failed to complete."
    SUCCESS=0
fi

if [ $SUCCESS -eq 1 ]; then
    if [ $UPDATED -gt 0 ]; then
        echo "i:success;m:$MESSAGE;s:0" > "$TEMPFILE"
    else
        echo "i:uptodate;m:$MESSAGE;s:0" > "$TEMPFILE"
    fi
else
    echo "i:failed;m:$MESSAGE;s:0" > "$TEMPFILE"
fi
