#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021, Antonio Niño Díaz

set -e

# Build tools

pushd tools/bin2c
rm -rf build ; mkdir build ; cd build
cmake ..
make
popd

pushd umod-player
rm -rf build ; mkdir build ; cd build
cmake ..
make
popd

# Paths to tools

export SUPERFAMICONV=${PWD}/SuperFamiconv/build/superfamiconv
export BIN2C=${PWD}/tools/bin2c/build/bin2c
export UMOD_PACKER=${PWD}/umod-player/build/packer/umod_packer

# Run child scripts

(cd examples && bash assets.sh)

# Done!

exit 0
