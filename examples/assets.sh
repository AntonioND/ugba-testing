#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021, Antonio Niño Díaz

set -e

(cd bios && bash assets.sh)
(cd dma && bash assets.sh)
(cd graphics && bash assets.sh)
(cd interrupts && bash assets.sh)
(cd sound && bash assets.sh)

exit 0
