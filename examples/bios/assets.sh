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

(cd bios_decompress && bash assets.sh)
(cd intr_wait && bash assets.sh)

exit 0
