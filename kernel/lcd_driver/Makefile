# Makefile to build only the panel driver from the kernel source

KERNEL_SRC := /usr/src/linux-headers-6.12.51-current-sunxi/drivers/gpu/drm/panel/
KERNEL_ARCH := arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
KCFLAGS=-g0

obj-m := panel-mtf0397swi.o
SRC := $(shell pwd) 

# Path to the panel driver, adjust as needed
PANEL_DRIVER_PATH := drivers/gpu/drm/panel

all:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC) 

#KBUILD_BTF=no

modules_install:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC) O=$(OBJDIR) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers