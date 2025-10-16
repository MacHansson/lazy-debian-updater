#!/usr/bin/env bash
# Scripts and app in: 	/usr/local/sbin/
# Autostart:		/etc/xdg/autostart/debie.desktop
# Systemd service: 	No timer/service necessary
# Logs in: 		/var/log/debie/
# Temp file: 		/var/tmp/debie
# User config:	 	~/.config/debie
# App config:		<app-directory>/config

source "$(dirname "$(realpath "$0")")/config"

set -Eeuo pipefail # crash on any error
IFS=$'\n\t' # better word-splitting
if [[ $EUID -ne 0 ]]; then # Relaunch as root and set user config directory
    exec sudo --preserve-env=PATH "$0" "$@"
fi
export DEBIAN_FRONTEND=noninteractive # set non interactive mode

echo "i:working:100;s:1" > "$TEMPFILE"

RESULT_APT=3
RESULT_FLATPAK=3
TIMESTAMP=$(date +'%Y-%m-%d-%H-%M-%S') 
mkdir -p "$LOGDIR"

start_apt_xterm() { # Function for starting update script in user session, using debie app
    echo "i:working-xterm:100;c:xterm:/usr/local/sbin/run-apt-xterm.sh" > "$TEMPFILE"
    touch $APTKEPTBACKFLAG
    exit 0
}

rename_log_file() {
    local OLD="$1"
    local NEW="${OLD/log-temp/$TIMESTAMP}" # replace last-log with timestamp
    local RESULT="$2"
    if [ -s "$OLD" ]; then
        mv "$OLD" "$NEW"
        if [ "$RESULT" -gt 1 ]; then
            mv "$NEW" "$NEW-error"
        fi
    fi
}

echo "$TIMESTAMP" > "$LASTRUN"
echo "[INFO] Starting automatic update: ($0)" >> "$LOG_APT"
echo "[INFO] Running apt-get update" >> "$LOG_APT"
apt-get update -y >> "$LOG_APT" 2>&1

# apt upgrade
if [ -z "$(apt-show-versions -u)" ] && [ ! -f $APTKEPTBACKFLAG ]; then
    RESULT_APT=0
    rm $LOG_APT
else
    RESULT_APT=1
    echo "[INFO] Running upgrade" >> "$LOG_APT"
    apt-get upgrade -y >> "$LOG_APT" 2>&1

    echo "[INFO] Running apt-get autoremove and clean" >> "$LOG_APT"
    apt-get autoremove -y >> "$LOG_APT" 2>&1
    apt-get clean >> "$LOG_APT" 2>&1
fi

# Flatpak update
flatpak update -y | grep -vE 'Looking for updates|Nothing to do' > "$LOG_FLATPAK" 2>&1 # Remove this lines
if [ "$(wc -l < $LOG_FLATPAK)" -gt 1 ]; then
    if ! grep -Eiq 'error|failed' "$LOG_FLATPAK"; then
        RESULT_FLATPAK=1
    else
        RESULT_FLATPAK=3
    fi
else
    RESULT_FLATPAK=0
    rm $LOG_FLATPAK
fi

# Check if apt upgrade was done
if [ ! -z "$(apt-show-versions -u)" ] && [ -f "$LOG_APT" ]; then
    if grep -nw "$LOG_APT" -e "kept back"; then
        if [ -f $APTKEPTBACKFLAG ]; then # If file exists, then packages are still kept back -> failed
            RESULT_APT=2
        else # If something was kept back - start script in xterm (interactive)
            echo "[INFO] Starting update script for apt in xterm" >> "$LOG_APT"
            start_apt_xterm
        fi
    else
        RESULT_APT=3
    fi
fi

if [ -f $APTKEPTBACKFLAG ]; then # Remove if exists (= this script runs the 2nd time)
    rm $APTKEPTBACKFLAG
fi

rename_log_file "$LOG_APT" "$RESULT_APT" # Add timestamp and -error postfix
rename_log_file "$LOG_FLATPAK" "$RESULT_FLATPAK"

bash "$(dirname "$(realpath "$0")")/show-result.sh" $RESULT_APT $RESULT_FLATPAK
