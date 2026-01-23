#!/bin/bash

cp panel-mtf0397swi.c /home/patrick/moredata/emmc/armbian_build/cache/sources/linux-kernel-worktree/6.6__sunxi__armhf/drivers/gpu/drm/panel

# obj-$(CONFIG_DRM_PANEL_MTF0397SWI) += panel-mtf0397swi.o


#config CONFIG_DRM_PANEL_MTF0397SWI
#    tristate "Microtech Technology MTF0397SWI-06 panel"
#    depends on DRM_MIPI_DSI
#    help
#      Say Y here if you want to enable support for Microtech Technology MTF0397SWI panel.
