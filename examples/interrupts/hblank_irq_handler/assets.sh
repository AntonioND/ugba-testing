#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021, Antonio Niño Díaz

set -e

# Prepare destination folder

OUT_DIR=built_assets
rm -rf ${OUT_DIR}
mkdir ${OUT_DIR}

GRAPHICS="${PWD}/graphics"

# Convert graphics

mkdir ${OUT_DIR}/graphics
pushd ${OUT_DIR}/graphics

grit ${GRAPHICS}/city.png -ftc -o city

popd

# Done!

exit 0
