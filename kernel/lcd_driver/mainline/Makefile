# Makefile to build only the panel driver from the kernel source

#KERNEL_SRC := /home/patrick/moredata/emmc/armbian_build/cache/sources/linux-kernel-worktree/6.6__sunxi__armhf
KERNEL_SRC := /home/patrick/moredata/emmc/armbian_build/cache/sources/linux-kernel-worktree/6.12__sunxi__armhf
KERNEL_ARCH := arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
KCFLAGS=-g0

obj-m := panel-mtf0397swi.o
SRC := $(shell pwd) 

# Path to the panel driver, adjust as needed
PANEL_DRIVER_PATH := drivers/gpu/drm/panel

all:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC) O=$(OBJDIR) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers