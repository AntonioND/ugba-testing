#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021, Antonio Niño Díaz

set -e

(cd basic_dma && bash assets.sh)
(cd hblank_dma_repeat && bash assets.sh)
(cd invalid_dma && bash assets.sh)
(cd vblank_dma_repeat && bash assets.sh)
(cd vblank_hblank_dma && bash assets.sh)

exit 0
