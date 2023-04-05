#!/usr/bin/env bash
# Alexis Megas.

if [ ! -r glitch.pro ]; then
    echo "Please execute $0 from the primary directory."
    exit 1
fi

# Prepare ./glitch.

make distclean 2>/dev/null
mkdir -p ./glitch/Documentation
mkdir -p ./glitch/Lib
qmake -o Makefile glitch.pro
lupdate glitch.pro
lrelease glitch.pro
make -j $(nproc)
cp -p ./Documentation/*.1 ./glitch/Documentation/.
cp -p ./Glitch ./glitch/.
cp -p ./Icons/Logo/glitch-logo.png ./glitch/.
cp -p ./glitch.sh ./glitch/.
cp -pr ./Documentation/Arduino ./glitch/Documentation/.

# Prepare a tar bundle.

cp $(ldd ./Glitch | awk '{print $3}' | grep -e '^/') ./glitch/Lib/.
chmod -x ./glitch/Lib/*.so*
tar -cv -f Glitch-2023.04.30.tar ./glitch
make distclean
rm -fr ./glitch
