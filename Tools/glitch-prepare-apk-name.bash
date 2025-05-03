#!/usr/bin/env bash

# Alexis Megas.

VERSION=$1

if [ -z "$VERSION" ]
then
    echo "Please specify the version: $0 <VERSION>."
    exit 1
fi

for i in $(find . -iname '*.apk')
do
    if [[ "$i" == *"arm64"* ]]
    then
	mv $i ~/Glitch-"$VERSION"_arm64.apk

	if [ -e ~/Glitch-"$VERSION"_arm64.apk ]
	then
	    echo "Created ~/Glitch-"$VERSION"_arm64.apk."
	fi
    fi

    if [[ "$i" == *"armeabi"* ]]
    then
	mv $i ~/Glitch-"$VERSION"_arm32.apk

	if [ -e ~/Glitch-"$VERSION"_arm32.apk ]
	then
	    echo "Created ~/Glitch-"$VERSION"_arm32.apk."
	fi
    fi
done
