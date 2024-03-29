#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021-2022, Antonio Niño Díaz

set -e

SCRIPT=`realpath $0`
IN=`dirname $SCRIPT`

echo ""
echo "[*] Converting ${IN}..."
echo ""

# Prepare destination folder

OUT_DIR=built_assets
rm -rf ${OUT_DIR}
mkdir ${OUT_DIR}

# Convert data

cp -r data built_assets

# Convert binary files generated by other stages

for dir in $(find ${OUT_DIR} -type d)
do
    for f in $(find $dir -maxdepth 1 -iname *.bin)
    do
        ${BIN2C} "$f" "$dir"
    done
done

# Done!

exit 0
