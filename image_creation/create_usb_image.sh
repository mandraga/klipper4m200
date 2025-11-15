#!/bin/bash
# Assuming we have a usb drive with the Armbian on it.
# Copy the dtb, u-boot, u-boot-env.txt (the zImage can come from Armbian).

# Give the usb disks
lsblk -d -o NAME,SIZE,MODEL,TRAN | grep usb
echo "Please enter the device name of the USB drive to use (e.g., sda):"
read DISK_DEV
#DISK_DEV=$(lsblk -d -o NAME,SIZE,MODEL,TRAN | grep usb | awk '{print $1}')
lsblk | grep $DISK_DEV

# mount Armbian rootfs partition and copy files
PART_ROOT="/dev/${DISK_DEV}1"
DIR_INSTALL="/mnt/usb_rootfs"
mkdir -p $DIR_INSTALL
mount $PART_ROOT $DIR_INSTALL
cp uboot.env  $DIR_INSTALL/boot
cp u-boot-dtb.bin $DIR_INSTALL/boot
cp sun8i-a33-zortrax-m200plus.dtb $DIR_INSTALL/boot/dtb
cp env.img  $DIR_INSTALL/
cp install_u-boot-mainline.sh $DIR_INSTALL/bin/
sync
umount $DIR_INSTALL
