#!/usr/bin/env sh
# Alexis Megas.

export AA_ENABLEHIGHDPISCALING=1
export AA_USEHIGHDPIPIXMAPS=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_X11_NO_MITSHM=1

if [ -r ./Glitch ] && [ -x ./Glitch ]
then
    echo "Launching a local Glitch."

    if [ -r ./Lib ]
    then
	export LD_LIBRARY_PATH=Lib
    fi

    exec ./Glitch "$@"
    exit $?
elif [ -r /opt/glitch/Glitch ] && [ -x /opt/glitch/Glitch ]
then
    echo "Launching an official Glitch."
    cd /opt/glitch && exec ./Glitch "$@"
    exit $?
elif [ -r /usr/local/glitch/Glitch ] && [ -x /usr/local/glitch/Glitch ]
then
    echo "Launching an official Glitch."
    cd /usr/local/glitch && exec ./Glitch "$@"
    exit $?
else
    echo "Cannot locate Glitch. Why?"
    exit 1
fi
