#!/usr/bin/env sh

# Alexis Megas.

export AA_ENABLEHIGHDPISCALING=1
export AA_USEHIGHDPIPIXMAPS=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_X11_NO_MITSHM=1

if [ -r ./Glitch ] && [ -x ./Glitch ]
then
    echo "Launching a local Glitch."
    ./Glitch "$@"
    exit $?
elif [ -r /opt/glitch/Glitch ] && [ -x /opt/glitch/Glitch ]
then
    echo "Launching an official Glitch."
    /opt/glitch/Glitch "$@"
    exit $?
elif [ -r /usr/local/glitch/Glitch ] && [ -x /usr/local/glitch/Glitch ]
then
    echo "Launching an official Glitch."
    /usr/local/glitch/Glitch "$@"
    exit $?
else
    echo "Cannot locate Glitch. Why?"
    exit 1
fi
