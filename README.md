<img src="https://github.com/BueniaDev/Botsashi/blob/main/res/logo.png" alt="drawing" width="275"/>

[![Run on Repl.it](https://repl.it/badge/github/BueniaDev/Botsashi)](https://repl.it/github/BueniaDev/Botsashi)
[![CircleCI](https://circleci.com/gh/BueniaDev/Botsashi.svg?style=svg)](https://circleci.com/gh/BueniaDev/Botsashi)

Motorola 68000 emulation engine, kinda

An emulation library for the iconic Motorola 68000 processor, written in C++17.

## Features

Platform-independent and architecture-independent code

Easily customizable interface

Dynamic disassembly (from any memory address) and simple debug output

And more to come!

## Simsashi

This repo comes with a custom (and currently WIP) reimplementation of the EASy68k simulation framework designed to test the Botsashi engine.

# Building Instructions

The core Botsashi library does not have any dependencies and can be compiled with MinGW and Clang on Windows, both GCC and Clang on Linux, and (presumably) AppleClang on OSX. However, the Simsashi simulator in this repo does have additional dependencies that need to be installed. All dependencies should be findable via CMake.

In order to build the Simsashi simulator, simply pass `-DBUILD_SIMSASHI="ON"` to CMake.

## Linux:

Step 1: Install dependencies:

Core dependencies:

* Compiler: GCC or Clang. You only need one of those two:

    * GCC 9.3.0+ (earlier versions not tested):

        * Debian: `sudo apt-get install build-essential`

        * Arch (not tested): `sudo pacman -S base-devel`

        * Fedora (not tested): `sudo dnf install gcc-c++`

        * OpenSUSE (not tested): `sudo zypper in gcc-c++`

    * Clang 10.0.0+ (earlier versions not tested):

        * Debian: `sudo apt-get install clang clang-format libc++-dev` (in some distros, clang-5.0)

        * Arch (not tested): `pacman -S clang`, `libc++` is in the AUR. Use pacaur or yaourt to install it.

        * Fedora (not tested): `dnf install clang libcxx-devel`

        * OpenSUSE (not tested): `zypper in clang`

* Git (if not installed already) and CMake 3.10+:

    * Debian: `sudo apt-get install git cmake`

    * Arch (not tested): `sudo pacman -S git cmake`

    * Fedora (not tested): `sudo dnf install git cmake`

    * OpenSUSE (not tested): `sudo zypper in git cmake extra-cmake-modules`

Optional dependencies:

* Qt5 (for the Simsashi simulator):

    * Debian (not tested): `sudo apt-get install qtbase5-dev libqt5opengl5-dev qtmultimedia5-dev`

    * Arch (not tested): `sudo pacman -S qt5`

    * Fedora (not tested): `sudo dnf install qt5-qtmultimedia-devel`

    * OpenSUSE: (not tested): `sudo zypper in libQt5Multimedia5 libqt5-qtmultimedia-devel libQt5Concurrent-devel`

Step 2: Clone the repository:

```
git clone --recursive https://github.com/BueniaDev/Botsashi.git
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

```
git clone --recursive https://github.com/BueniaDev/Botsashi.git
cd Botsashi
```

Step 3: Compile:

```
mkdir build && cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="<Debug/Release>"
make -j$(sysctl -n hw.ncpu)
```

## Windows:

You will need [MSYS2](https://msys2.github.io) in order to install Botsashi.
Make sure to run `pacman -Syu` as needed.

Step 1: Install dependencies:

`pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake <mingw-w64-x86_64-qt5> git`

Step 2: Clone the repository:

```
git clone --recursive https://github.com/BueniaDev/Botsashi.git
cd Botsashi
```

Step 3: Compile:

```
mkdir build && cd build
cmake .. -G "MSYS Makefiles" -DBUILD_SIMSASHI="<ON/OFF>" -DCMAKE_BUILD_TYPE="<Debug/Release>"
(mingw32-)make -j$(nproc --all)
../msys-dist.sh
```

# Plans

## Near-term

Finish implementing M68K instruction set

Improve exception/IRQ support

Feature parity between the Simsashi and SIM68k simulators

## Medium-term

Feature parity with the [Musashi engine](https://github.com/kstenerud/Musashi)

## Long-term

Motorola 68010/68020 support

Graphical debugger (possibly via Qt5?)

# License

<img src="https://www.gnu.org/graphics/gplv3-127x51.png" alt="drawing" width="150"/>

Botsashi is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# Copyright

(C) 2022 BueniaDev. This project is not affiliated in any way with NXP or Motorola. Motorola 68000 is a registered trademark of NXP Semiconductors N.V.

All copyrighted material in this repository belongs to their respective owners, and is used under the terms of the "fair use" clause of U.S. and international copyright law. For more information, see the "res/fairuse.txt" file in this repository.

For information regarding Botsashi's stance on copyright infringement, see the DISCLAIMER.md file in this repository.
