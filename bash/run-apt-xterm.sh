#!/usr/bin/env bash
source "$(dirname "$(realpath "$0")")/config"
if [[ $EUID -ne 0 ]]; then
    exec sudo --preserve-env=PATH "$0" "$@"
fi
echo "* DEBIE - Debian Background Install Extension (2025)"
echo "* Do not close this window - attempting to install kept back apt packages"
echo ""
export DEBIAN_FRONTEND=noninteractive
apt-get update 2>&1 | tee "$LOG_APT"
apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" full-upgrade 2>&1 | tee "$LOG_APT"
bash /usr/local/sbin/run.sh &
echo ""
echo "* Checking for success (may take some seconds)"
for i in {3..1}; do
  echo "* Closing window in $i seconds..."
  sleep 1
done
