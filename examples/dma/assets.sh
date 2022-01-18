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

(cd basic_dma && bash assets.sh)
(cd hblank_dma_repeat && bash assets.sh)
(cd invalid_dma && bash assets.sh)
(cd vblank_dma_repeat && bash assets.sh)
(cd vblank_hblank_dma && bash assets.sh)

exit 0
