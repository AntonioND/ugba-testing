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

# Convert music

mkdir ${OUT_DIR}/audio

S3M2GBT=s3m2gbt.py

python3 ${S3M2GBT} \
    --input audio/template.s3m \
    --name template \
    --output ${OUT_DIR}/audio/template.c \
    --instruments

# Done!

exit 0
