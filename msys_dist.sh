#!/bin/bash

# if [[ ! -f "libbotsashi.a" ]]; then
#	echo "Run this script from the directory where you built the Botsashi engine."
#	exit 1
# fi

mkdir -p dist

if [ -d "example" ]; then
	for lib in $(ldd example/example.exe | grep mingw | sed "s/.*=> //" | sed "s/(.*)//"); do
		cp "${lib}" dist
	done
	cp example/example.exe dist
fi

