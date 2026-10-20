#!/usr/bin/env bash
# Alexis Megas.
# Qt LTS only please!

# Please execute this script in an Apple terminal because
# Apple may prompt for a certificate password (codesign).

if [ ! -e glitch.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi


if [ ! -z "${SSH_TTY}" ]
then
    echo "SSH session detected. " \
	 "MacOS codesign password prompt may be invisible."
fi

make distclean 1>/dev/null 2>/dev/null

declare -a qmakes=("$HOME/Qt/6.12.0/macos/bin/qmake"
		   "$HOME/Qt/6.8.3/macos/bin/qmake")
qmake=""

for i in "${qmakes[@]}"
do
    qmake="$(echo $i)"

    if [ -x "$qmake" ]
    then
	break
    fi
done

if [ -x "$qmake" ]
then
    echo "Found $qmake. Creating Makefile."
    $qmake -o Makefile glitch.pro 1>/dev/null 2>/dev/null
else
    echo "Cannot locate qmake. Please install the official Qt."
    exit 1
fi

rm -f Glitch.d.dmg
make -j $(sysctl -n hw.ncpu) 1>/dev/null 2>/dev/null
make install
codesign --deep --force -s "textbrowser" ./Glitch.d/Glitch.app
make dmg

if [ ! -r Glitch.d.dmg ]
then
    echo "Glitch.d.dmg is not a readable file."
    exit 1
fi

mv Glitch.d.dmg Glitch-2026.05.22_Universal.d.dmg
make distclean 2>/dev/null
