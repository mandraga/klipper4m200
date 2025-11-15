#/!bin/bash
# Copy the mainline U-Boot binary and env file to the FAT16 partition on the Zortrax m200+ emmc
DATA_DIR="/boot/"
mkdir -p /mnt/fat16disk
mount /dev/mmcblk2p2 /mnt/fat16disk
cp ${DATA_DIR}u-boot-dtb.bin /mnt/fat16disk/
cp ${DATA_DIR}uboot.env /mnt/fat16disk/
umount /mnt/fat16disk

# Change the sunxi u-boot env partition on the emmc to start mainline u-boot
dd if=${DATA_DIR}env.img of=/dev/mmcblk2p5 bs=1K seek=0 count=2
