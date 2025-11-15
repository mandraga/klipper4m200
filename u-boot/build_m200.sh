#!/bin/sh
make CROSS_COMPILE=arm-linux-gnueabihf- A33-ZortraxM200Plus_defconfig
make CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
cp u-boot-sunxi-with-spl.bin ../../sunxi-tools/
