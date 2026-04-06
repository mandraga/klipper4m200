#/!bin/bash
# Work in progress

# Copy the mainline U-Boot binary and env file to the FAT16 partition on the Zortrax m200+ emmc
DATA_DIR="/boot/"
mkdir -p /mnt/fat16disk
mount /dev/mmcblk2p2 /mnt/fat16disk
cp ${DATA_DIR}u-boot-dtb.bin /mnt/fat16disk/
umount /mnt/fat16disk

# Save the curent sunxi u-boot env partition to a file
dd if=/dev/mmcblk2p5 of=sunxi_env.img bs=1K seek=0 count=2

# Change the sunxi u-boot env partition on the emmc to start mainline u-boot
dd if=${DATA_DIR}sunxi_env.img of=/dev/mmcblk2p5 bs=1K seek=0 count=2
