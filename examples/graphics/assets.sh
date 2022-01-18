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

(cd background_affine && bash assets.sh)
(cd background_regular_16col && bash assets.sh)
(cd background_window && bash assets.sh)
(cd mosaic && bash assets.sh)
(cd priorities && bash assets.sh)
(cd special_effects && bash assets.sh)
(cd sprite_affine_16col && bash assets.sh)
(cd sprite_lowlevel_16col && bash assets.sh)
(cd sprite_lowlevel_256col && bash assets.sh)
(cd sprite_regular_16col && bash assets.sh)
(cd sprite_regular_256col && bash assets.sh)
(cd windows && bash assets.sh)

exit 0
