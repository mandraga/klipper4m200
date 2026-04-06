#!/bin/bash
# Work in progress

# Assuming we have a usb drive with the Armbian on it.
# Copy the dtb, u-boot, u-boot-env.txt (the zImage can come from Armbian).

# Use a parameter for the image file, or default to the given filename
IMG_FILE="${1:-Armbian-unofficial_25.11.0-trunk_Lime-a33_bookworm_legacy_6.6.75_minimal.img}"

if [ ! -f "$IMG_FILE" ]; then
    echo "Image file '$IMG_FILE' not found."
    exit 1
fi

# Create a loop device for the image
LOOP_DEV=$(losetup --show -Pf "$IMG_FILE")

# Find the rootfs partition (assume it's the first partition)
PART_ROOT="${LOOP_DEV}p1"
DIR_INSTALL="/tmp/armbian_image"
mkdir -p $DIR_INSTALL
mount $PART_ROOT $DIR_INSTALL

cp uboot.env  $DIR_INSTALL/boot
cp u-boot-dtb.bin $DIR_INSTALL/boot
cp sun8i-a33-zortrax-m200plus.dtb $DIR_INSTALL/boot/dtb
cp sunxi_env.img  $DIR_INSTALL/boot
cp install_u-boot-mainline.sh $DIR_INSTALL/bin/
sync

umount $DIR_INSTALL
losetup -d $LOOP_DEV
