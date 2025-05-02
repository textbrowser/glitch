#!/usr/bin/env bash

# Alexis Megas.

if [ ! -x /usr/bin/dpkg ]
then
    echo "Please install dpkg."
    exit 1
fi

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

# Prepare ./opt/glitch.

make distclean 2>/dev/null
mkdir -p ./opt/glitch/Documentation

if [ ! -z "$(which qmake)" ]
then
    qmake -o Makefile glitch.pro
else
    qmake6 -o Makefile glitch.pro
fi

lupdate glitch.pro 2>/dev/null
lrelease glitch.pro 2>/dev/null
make -j $(nproc)
cp -p ./Documentation/*.1 ./opt/glitch/Documentation/.
cp -p ./Glitch ./opt/glitch/.
cp -p ./Icons/Logo/glitch-logo.png ./opt/glitch/.
cp -p ./glitch.sh ./opt/glitch/.
cp -pr ./Documentation/Arduino ./opt/glitch/Documentation/.
cp -pr ./Examples ./opt/glitch/.

# Prepare Glitch-x.deb.

mkdir -p glitch-debian/opt

architecture="$(dpkg --print-architecture)"

if [ "$architecture" = "armhf" ]
then
    cp -pr ./Distributions/DEBIAN-PI-ARM32 glitch-debian/DEBIAN
else
    cp -pr ./Distributions/DEBIAN-PI-ARM64 glitch-debian/DEBIAN
fi

cp -r ./opt/glitch glitch-debian/opt/.
fakeroot dpkg-deb --build glitch-debian Glitch-2025.05.03_$architecture.deb
rm -fr ./opt
rm -fr glitch-debian
make distclean
