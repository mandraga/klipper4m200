# Image creation, binding Armbian, u-boot

## Blank partition

1. Create a blank image file

```
dd if=/dev/zero of=zortrax_armbian.img bs=1M count=7450
```

2. Create an MBR partition table and a ext4 partition

```
parted zortrax_armbian.img --script -- mklabel msdos mkpart primary ext4 1MiB 7445MiB
```

3. Set up loop devices:

```
sudo losetup -fP zortrax_armbian.img
# Find the loop device, e.g., /dev/loop0
Comment savoir quel disque c'est?????
```

4. Write U-Boot to the image (skip first 1KB for MBR):

```
sudo apt-get install mmc-utils
mmc bootbus set single_hs x1 x4 /dev/mmcblk2
mmc bootpart enable 1 1 /dev/mmcblk2

sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2 bs=1024 seek=8
```


5. Format the partition:

```
Ne fonctionne pas
sudo mkfs.ext4 /dev/loop0p1 -L armbian
```

7. Detach the loop device:

```
sudo losetup -d /dev/loop0
```

8. Create a bmap file for fast flashing:

```
bmaptool create zortrax_armbian.img > zortrax_armbian.img.bmap
```

9. Flash the image using bmaptool:

```
sudo bmaptool copy --bmap zortrax_armbian.img.bmap zortrax_armbian.img /dev/mmcblk1
```
