#!/bin/sh

export AA_ENABLEHIGHDPISCALING=1
export AA_USEHIGHDPIPIXMAPS=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_X11_NO_MITSHM=1

if [ -r ./Glitch ] && [ -x ./Glitch ]
then
    echo "Launching a local Glitch."
    exec ./Glitch "$@"
    exit $?
elif [ -r /usr/local/glitch/Glitch ] && [ -x /usr/local/glitch/Glitch ]
then
    cd /usr/local/glitch && exec ./Glitch "$@"
    exit $?
else
    exit 1
fi
