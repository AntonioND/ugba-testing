#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021-2022, Antonio Niño Díaz

set -e

# Paths to tools

export SUPERFAMICONV=${PWD}/SuperFamiconv/bin/superfamiconv
export BIN2C=${PWD}/tools/bin2c/build/bin2c
export UMOD_PACKER=${PWD}/umod-player/build/packer/umod_packer

# Build tools

if [ ! -d "${BIN2C}" ]; then
    pushd tools/bin2c
    rm -rf build ; mkdir build ; cd build
    cmake ..
    make -j`nproc`
    popd
fi

if [ ! -d "${SUPERFAMICONV}" ]; then
    pushd SuperFamiconv
    make -j`nproc`
    popd
fi

if [ ! -d "${UMOD_PACKER}" ]; then
    pushd umod-player
    rm -rf build ; mkdir build ; cd build
    cmake ..
    make -j`nproc`
    popd
fi

# Run child scripts

(cd examples && bash assets.sh)

# Done!

exit 0
