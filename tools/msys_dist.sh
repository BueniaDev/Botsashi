#!/bin/bash

# if [[ ! -f "libbotsashi.a" ]]; then
#	echo "Run this script from the directory where you built the Botsashi engine."
#	exit 1
# fi

mkdir -p dist

if [ -d "Simsashi" ]; then
	for lib in $(ldd Simsashi/simsashi.exe | grep mingw | sed "s/.*=> //" | sed "s/(.*)//"); do
		cp "${lib}" dist
	done
	cp Simsashi/simsashi.exe dist
	windeployqt dist/simsashi.exe
fi

