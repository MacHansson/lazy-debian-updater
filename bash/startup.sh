#!/bin/bash

#echo "Entering startup script: $(date +'%Y-%m-%d-%H-%M-%S')"

# Check if we're in a KDE graphical session
if [ "$XDG_SESSION_DESKTOP" != "KDE" ] || [ -z "$DISPLAY" ]; then
    #echo "Not in a KDE graphical session. Exiting."
    exit 1
fi

#echo "KDE session detected. Checking readiness..."

# Wait until plasmashell is running
while ! pgrep plasmashell > /dev/null; do
    #echo "Waiting for plasmashell to start..."
    sleep 2
done

# Wait until ksplashqml (KDE splash screen) exits
while pgrep ksplashqml > /dev/null; do
    #echo "Waiting for KDE to finish loading..."
    sleep 2
done

#echo "Starting application: $(date +'%Y-%m-%d-%H-%M-%S')"

/usr/local/sbin/debie-app &
sleep 3
bash /usr/local/sbin/run.sh
