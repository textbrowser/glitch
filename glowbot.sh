#!/bin/sh

if [ -r /usr/local/glowbot/GlowBot ] && [ -x /usr/local/glowbot/GlowBot ]
then
    cd /usr/local/glowbot && exec ./GlowBot -style fusion "$@"
    exit $?
else
    exit 1
fi
