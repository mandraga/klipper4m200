## Env to chainload u-boot

This env image for sunxi secondary loader chainloads u-boot from the FAT16 partition.
To install it from Armbian:

```
dd if=sunxi_env.img of=/dev/mmcblk2p5
```

And copy u-boot proper to mmcblk2p2:

```
cd /mnt/
mkdir -p /mnt/ext4disk
mkdir -p /mnt/fat16disk
mount /dev/sda1 ext4disk/
mount /dev/mmcblk2p2 fat16disk
cp ext4disk/u-boot-dtb.bin fat16disk/
sync
reboot
```
