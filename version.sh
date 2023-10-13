#!/usr/bin/env bash

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: $0 <VERSION>"
    exit 1
fi

for file in Distributions/*/control; do
    sed -i "s/Version: .*/Version: $VERSION/" $file
done

for file in Distributions/build*; do
    sed -i "s/Glitch-.*_/Glitch-$VERSION\_/" $file
done

FILE="Source/glitch-version.h"

sed -i 's/\(GLITCH_VERSION_STRING "\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE

FILE="android/AndroidManifest.xml"

sed -i \
    's/\(android:versionName="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
sed -i \
    's/\(android:versionCode="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"${VERSION//./}"'"/' \
    $FILE

echo "Please modify ReleaseNotes.html."
