<img src="https://github.com/BueniaDev/Botsashi/blob/rewrite/res/logo.png" alt="drawing" width="275"/>

[![Run on Repl.it](https://repl.it/badge/github/BueniaDev/Botsashi)](https://repl.it/github/BueniaDev/Botsashi)

Motorola 68000 emulation engine, kinda

This was created to emulate the M68K processor. But also, you know, to have a fun challenge ;)

## History

This was originally created back in 2018, when I was still a high-school student. However, in order to modernize and improve the codebase, the Botsashi engine is currently being rewritten at the moment. If you wish to look at the code from before the rewrite, check out the current master branch.

## Simsashi

This repo comes with a custom (and currently WIP) reimplementation of the EASy68k simulation framework designed to test the Botsashi engine.

# Building Instructions

The core Botsashi library does not have any dependencies and can be compiled with MinGW on Windows, and both GCC and (presumably) Clang on Linux, as well as (presumably) AppleClang on OSX. However, the Simsashi simulator in this repo does have additional dependencies that need to be installed. All dependencies should be findable via CMake.

In order to build the Simsashi simulator, simply pass `-DBUILD_SIMSASHI="ON"` to CMake.

## Linux:

Step 1: Install dependencies:

Core dependencies:

* Compiler: GCC or Clang. You only need one of those two:

    * GCC 7.5.0+ (earlier versions not tested):

        * Debian: `sudo apt-get install build-essential`

        * Arch (not tested): `sudo pacman -S base-devel`

        * Fedora (not tested): `sudo dnf install gcc-c++`

        * OpenSUSE (not tested): `sudo zypper in gcc-c++`

    * Clang (not tested):

        * Debian: `sudo apt-get install clang clang-format libc++-dev` (in some distros, clang-5.0)

        * Arch: `pacman -S clang`, `libc++` is in the AUR. Use pacaur or yaourt to install it.

        * Fedora: `dnf install clang libcxx-devel`

        * OpenSUSE: `zypper in clang`

* Git (if not installed already) and CMake 3.15+:

    * Debian: `sudo apt-get install git cmake`

    * Arch (not tested): `sudo pacman -S git cmake`

    * Fedora (not tested): `sudo dnf install git cmake`

    * OpenSUSE (not tested): `sudo zypper in git cmake extra-cmake-modules`

Optional dependencies:

    * SDL2 (for the Simsashi simulator):

        * Debian: `sudo apt-get install libsdl2-dev`

        * Arch (not tested): `sudo pacman -S sdl2`

        * Fedora (not tested): `sudo dnf install SDL2-devel`

        * OpenSUSE: (not tested): `sudo zypper in libSDL2-devel`

Step 2: Clone the repository:

```
git clone --recursive https://github.com/BueniaDev/libmbGB.git
cd Botsashi
```

Step 3: Compile:

```
mkdir build && cd build
cmake .. -G "Unix Makefiles" -DBUILD_SIMSASHI="<ON/OFF>" -DCMAKE_BUILD_TYPE="<Debug/Release>"
make -j$(nproc --all)
```


## Mac OS (not tested):

You will need [homebrew](https://brew.sh), a recent version of Xcode and the Xcode command-line tools to build Botsashi.
Please note that due to personal financial constraints, Botsashi has not been tested on Mac OS as of yet.

Step 1: Install dependencies:

`brew install git cmake pkg-config`

Step 2: Clone the repository:

`git clone --recursive https://github.com/BueniaDev/Botsashi.git`

`cd Botsashi`

Step 3: Compile:

`mkdir build && cd build`

`cmake .. -G "Unix Makefiles" -DBUILD_SIMSASHI="<ON/OFF>" -DCMAKE_BUILD_TYPE="<Debug/Release>"`

`make -j$(sysctl -n hw.ncpu)`

## Windows:

You will need [MSYS2](https://msys2.github.io) in order to install Botsashi.
Make sure to run `pacman -Syu` as needed.

Step 1: Install dependencies:

`pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-SDL2 git`

Step 2: Clone the repository:

`git clone --recursive https://github.com/BueniaDev/Botsashi.git`

`cd Botsashi`

Step 3: Compile:

`mkdir build && cd build`

`cmake .. -G "MSYS Makefiles" -DBUILD_SIMSASHI="<ON/OFF>" -DCMAKE_BUILD_TYPE="<Debug/Release>"`

`(mingw32-)make -j$(nproc --all)`

`../msys-dist.sh`

# Plans

## Near-term

Complete Motorola 68000 support (as of right now, this engine supports only a tiny subset of the M68K instruction set without any interrupt support)

# License

<img src="https://www.gnu.org/graphics/gplv3-127x51.png" alt="drawing" width="150"/>

Botsashi is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# Copyright

(C) 2021 BueniaDev. This project is not affiliated in any way with NXP or Motorola. Motorola 68000 is a registered trademark of NXP Semiconductors N.V.

All copyrighted material in this repository belongs to their respective owners, and is used under the terms of the "fair use" clause of U.S. and international copyright law. For more information, see the "res/fairuse.txt" file in this repository.

For information regarding Botsashi's stance on copyright infringement, see the DISCLAIMER.md file in this repository.