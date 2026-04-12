# Build u-boot

Find a compatible u-boot in [my u-boot fork](git@github.com:mandraga/u-boot.git)
The device tree is in this repository.
It is also in the submodules of this repo.


# Change the sunxi boot env to chainload u-boot

The modified sunxi u-boot env replaces the stock env, it adds a one second wait to enter the u-boot console instead of zero, and chain loads u-boot mainline as default boot command.

## Install the env to chainload u-boot

This env image for sunxi secondary loader chainloads u-boot from the FAT16 partition.
To install it from Armbian:

```
dd if=sunxi_env.img of=/dev/mmcblk2p5
```

## Install u-boot on the FAT16 partition

Copy u-boot proper to mmcblk2p2:

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



