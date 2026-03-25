#!/usr/bin/env bash
if [[ $EUID -ne 0 ]]; then
    exec sudo --preserve-env=PATH "$0" "$@"
fi
source "$(dirname "$(realpath "$0")")/config"
LOCKFILE="/tmp/debie-run-apt-xterm.lock"
trap "rm -f $LOCKFILE" EXIT
if [ -e "$LOCKFILE" ]; then
    echo "Script already running. Exiting."
    rm -f "$LOCKFILE"
    exit 1
fi
touch "$LOCKFILE"
echo "* DEBIE - Debian Background Install Extension (2025)"
echo "* Do not close this window - attempting to install kept back apt packages"
echo ""
export DEBIAN_FRONTEND=noninteractive
apt-get update 2>&1 | tee "$LOG_APT"
apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" full-upgrade 2>&1 | tee "$LOG_APT"
echo "* Installing new graphics driver *"
for i in {3..1}; do
  echo "* Continue in $i seconds..."
  sleep 1
done
bash /usr/local/sbin/nvidia.sh
for i in {3..1}; do
  echo "* Continue in $i seconds..."
  sleep 1
done
bash /usr/local/sbin/run.sh &
echo ""
echo "* Checking for success (may take some seconds)"
echo ""
echo "* Installation complete. Press any key to exit."
read -rsn1 -p ""
echo "Leaving ..."
sleep 1
