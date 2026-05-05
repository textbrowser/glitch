#!/usr/bin/env bash

# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]
then
    echo "Please install dpkg-deb."
    exit 1
fi

if [ ! -x /usr/bin/fakeroot ]
then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -r glitch.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

missing=1

if [ ! -z "$(which qmake)" ]
then
    missing=0
fi

if [ ! -z "$(which qmake5)" ]
then
    missing=0
fi

if [ ! -z "$(which qmake6)" ]
then
    missing=0
fi

if [ $missing -eq 1 ]
then
    echo "Missing qmake, qmake5, or qmake6."
    exit 1
fi

# Prepare ./opt/glitch.

make distclean 2>/dev/null
mkdir -p ./opt/glitch/Documentation

if [ ! -z "$(which qmake6)" ]
then
    qmake6 -o Makefile glitch.pro
elif [ ! -z "$(which qmake5)" ]
then
    qmake5 -o Makefile glitch.pro
else
    qmake -o Makefile glitch.pro
fi

lupdate glitch.pro 2>/dev/null
lrelease glitch.pro 2>/dev/null
make -j 5
cp -p ./Documentation/*.1 ./opt/glitch/Documentation/.
cp -p ./Glitch ./opt/glitch/.
cp -p ./Icons/Logo/glitch-logo.png ./opt/glitch/.
cp -p ./glitch.sh ./opt/glitch/.
cp -pr ./Documentation/Arduino ./opt/glitch/Documentation/.
cp -pr ./Examples ./opt/glitch/.

# Prepare Glitch-x.deb.

mkdir -p glitch-debian/opt
cp -pr ./Distributions/DEBIAN glitch-debian/.
cp -r ./opt/glitch glitch-debian/opt/.
fakeroot dpkg-deb --build glitch-debian Glitch-2026.05.10_amd64.deb
rm -fr ./opt
rm -fr glitch-debian
make distclean
