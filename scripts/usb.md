## UMS mode

Uboot has a UMS mode such as the emmc becomes a usb mass storage device. But I could not mke it work on the m200. The command was not visible and if the otg was enabled in the device tree, uboot crashed.
Plus the USB0 port has weird settings with a regulator at 3V (see the fex file).

## Transfering an image without usb

Until the UMS mode works, we use scripts to break the image into 16MB files and load them one by one from a memory stick to the EMMC1 using the serial port to do the reassembling.

## Transfer using linux

In fact we can boot an image from the usb drive and use this image to flash the device.

```
setenv bootargs root=/dev/sda1 rootwait console=ttyS0,115200
usb start
fatload usb 0:1 0x42000000 zImage
fatload usb 0:1 0x43000000 sun8i-a33-zortrax-m200.dtb
bootz 0x42000000 - 0x43000000
```

Login and then dump the firmware on the USB board

```
cd /mnt
mkdir ext4disk
mount /dev/sda2 ext4disk/
dd if=/dev/mmcblk1 of=ext4disk/emmc.image bs=4M status=progress
sync
poweroff
```
