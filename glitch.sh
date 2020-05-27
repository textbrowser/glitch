#!/bin/sh

export QT_AUTO_SCREEN_SCALE_FACTOR=1

if [ -r ./Glitch ] && [ -x ./Glitch ]
then
    exec ./Glitch "$@"
    exit $?
elif [ -r /usr/local/glitch/Glitch ] && [ -x /usr/local/glitch/Glitch ]
then
    cd /usr/local/glitch && exec ./Glitch "$@"
    exit $?
else
    exit 1
fi
