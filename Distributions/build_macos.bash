#!/usr/bin/env bash
# Alexis Megas.

if [ ! -x ~/Qt/6.5.3/macos/bin/qmake ]; then
    echo "Please install the official Qt."
    exit
fi

make distclean 2>/dev/null
rm -f Glitch.d.dmg
~/Qt/6.5.3/macos/bin/qmake -o Makefile
make -j 5
make install
make dmg

if [ "$(uname -m)" == "arm64" ]; then
    mv Glitch.d.dmg Glitch-2024.09.25_apple_silicon.d.dmg
else
    mv Glitch.d.dmg Glitch-2024.09.25_intel.d.dmg
fi
