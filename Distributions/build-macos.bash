#!/usr/bin/env bash

# Alexis Megas.

if [ ! -e glitch.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

if [ ! -x ~/Qt/6.8.3/macos/bin/qmake ]
then
    echo "Please install the official Qt LTS."
    exit 1
fi

make distclean 2>/dev/null
rm -f Glitch.d.dmg
~/Qt/6.8.3/macos/bin/qmake -o Makefile
make -j 5
make dmg

if [ ! -r Glitch.d.dmg ]
then
    echo "Glitch.d.dmg is not a readable file."
    exit 1
fi

mv Glitch.d.dmg Glitch-2025.05.03_universal.d.dmg
make distclean 2>/dev/null
