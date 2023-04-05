#!/usr/bin/env bash
# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]; then
    echo "Please install dpkg-deb."
    exit
fi

if [ ! -x /usr/bin/fakeroot ]; then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -r glitch.pro ]; then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

# Preparing ./usr/local/glitch:

make distclean 2>/dev/null
mkdir -p ./usr/local/glitch/Documentation
qmake -o Makefile glitch.pro
lupdate glitch.pro
lrelease glitch.pro
make -j $(nproc)
cp -p ./Documentation/*.1 ./usr/local/glitch/Documentation/.
cp -p ./Glitch ./usr/local/glitch/.
cp -p ./Icons/Logo/glitch-logo.png ./usr/local/glitch/.
cp -p ./glitch.sh ./usr/local/glitch/.
cp -pr ./Documentation/Arduino ./usr/local/glitch/Documentation/.

# Preparing Glitch-x.deb:

mkdir -p glitch-debian/usr/local
cp -pr ./DEBIAN glitch-debian/.
cp -r ./usr/local/glitch glitch-debian/usr/local/.
fakeroot dpkg-deb --build glitch-debian Glitch-2023.04.30_amd64.deb
rm -fr ./usr
rm -fr glitch-debian
make distclean
