```
U-Boot SPL 2025.04-rc5 (Oct 19 2025 - 19:35:59 +0200)
DRAM: 1024 MiB
Trying to boot from FEL


U-Boot 2025.04-rc5 (Oct 19 2025 - 19:35:59 +0200) Allwinner Technology

CPU:   Allwinner A33 (SUN8I 1667)
Model: Zortrax A33-M200
DRAM:  1 GiB
Core:  61 devices, 21 uclasses, devicetree: separate
WDT:   Not starting watchdog@1c20ca0
MMC:   mmc@1c11000: 1
Loading Environment from FAT... ** Bad device specification mmc 0 **
Unknown monitor
Unknown monitor
In:    serial,usbkbd
Out:   serial,vidconsole
Err:   serial,vidconsole
Net:   No ethernet found.

starting USB...
Bus usb@1c1a000: USB EHCI 1.00
Bus usb@1c1a400: USB OHCI 1.0
scanning bus usb@1c1a000 for devices... 1 USB Device(s) found
scanning bus usb@1c1a400 for devices... 1 USB Device(s) found
       scanning usb for storage devices... 0 Storage Device(s) found
Hit any key to stop autoboot:  0 
=> help
?         - alias for 'help'
base      - print or set address offset
bdinfo    - print Board Info structure
bind      - Bind a device to a driver
blkcache  - block cache diagnostics and control
boot      - boot default, i.e., run 'bootcmd'
bootd     - boot default, i.e., run 'bootcmd'
bootefi   - Boots an EFI payload from memory
bootelf   - Boot from an ELF image in memory
bootflow  - Boot flows
bootm     - boot application image from memory
bootp     - boot image via network using BOOTP/TFTP protocol
bootvx    - Boot vxWorks from an ELF image
bootz     - boot Linux zImage image from memory
cls       - clear screen
cmp       - memory compare
coninfo   - print console devices and information
cp        - memory copy
crc32     - checksum calculation
cyclic    - Cyclic
dhcp      - boot image via network using DHCP/TFTP protocol
dm        - Driver model low level access
echo      - echo args to console
editenv   - edit environment variable
eficonfig - provide menu-driven UEFI variable maintenance interface
env       - environment handling commands
exit      - exit script
ext2load  - load binary file from a Ext2 filesystem
ext2ls    - list files in a directory (default /)
ext4load  - load binary file from a Ext4 filesystem
ext4ls    - list files in a directory (default /)
ext4size  - determine a file's size
false     - do nothing, unsuccessfully
fatinfo   - print information about filesystem
fatload   - load binary file from a dos filesystem
fatls     - list files in a directory (default /)
fatmkdir  - create a directory
fatrm     - delete a file
fatsize   - determine a file's size
fatwrite  - write file into a dos filesystem
fdt       - flattened device tree utility commands
fstype    - Look up a filesystem type
fstypes   - List supported filesystem types
go        - start application at address 'addr'
gpio      - query and control gpio pins
gpt       - GUID Partition Table
help      - print command description/usage
iminfo    - print header information for application image
imxtract  - extract a part of a multi-image
itest     - return true/false on integer compare
lcdputs   - print string on video framebuffer
ln        - Create a symbolic link
load      - load binary file from a filesystem
loadb     - load binary file over serial line (kermit mode)
loads     - load S-Record file over serial line
loadx     - load binary file over serial line (xmodem mode)
loady     - load binary file over serial line (ymodem mode)
loop      - infinite loop on address range
ls        - list files in a directory (default /)
md        - memory display
mii       - MII utility commands
mm        - memory modify (auto-incrementing address)
mmc       - MMC sub system
mmcinfo   - display MMC info
mw        - memory write (fill)
net       - NET sub-system
nm        - memory modify (constant address)
panic     - Panic with optional message
part      - disk partition related commands
ping      - send ICMP ECHO_REQUEST to network host
pinmux    - show pin-controller muxing
poweroff  - Perform POWEROFF of the device
printenv  - print environment variables
pxe       - get and boot from pxe files
random    - fill memory with random pattern
reset     - Perform RESET of the CPU
run       - run commands in an environment variable
save      - save file to a filesystem
saveenv   - save environment variables to persistent storage
setcurs   - set cursor position within screen
setenv    - set environment variables
setexpr   - set environment variable as the result of eval expression
showvar   - print local hushshell variables
size      - determine a file's size
sleep     - delay execution for some time
source    - run script from memory
sysboot   - command to get and boot from syslinux files
test      - minimal test like /bin/sh
tftpboot  - load file via network using TFTP protocol
true      - do nothing, successfully
unbind    - Unbind a device from a driver
usb       - USB sub-system
usbboot   - boot from USB device
version   - print monitor, compiler and linker version
=> 


=> part list usb 0

Partition Map for usb device 0  --   Partition Type: DOS

Part	Start Sector	Num Sectors	UUID		Type
  1	32        	30031840  	00000000-01	0c

mmc read <ram_addr> <start_sector> <num_sectors>
mmc read 0x42000000 1 0x2D000
fatwrite usb 0:1 0x42000000 boot0.img 
mmc read 0x42000000 0x2D001 0x2D000
fatwrite usb 0:1 0x42000000 boot1.img 
mmc read 0x42000000 0x5A001 0x2D000
...
mmc read 0x42000000 0x276001 0x2D000
fatwrite usb 0:1 0x42000000 boot14.img size in 32bits!!!
mmc read 0x42000000 0x2A3001 0x2D000
fatwrite usb 0:1 0x42000000 boot15.img 

total 1509949440   0x2D0000 secteurs
188743680 octets pour 0x2D0000 / 8



   # Read boot image to RAM (into $loadaddr)
=> part start mmc 1 boot boot_start
=> part size mmc 1 boot boot_size
=> mmc read $loadaddr $boot_start $boot_size

   # Read DTBO image to RAM (into $dtboaddr)
=> part start mmc 1 dtbo dtbo_start
=> part size mmc 1 dtbo dtbo_size
=> mmc read $dtboaddr $dtbo_start $dtbo_size

   # Copy required DTB blob (into $fdtaddr)
=> abootimg get dtb --index=0 dtb0_start dtb0_size
=> cp.b $dtb0_start $fdtaddr $dtb0_size

   # Merge required DTBO blobs into DTB blob
=> fdt addr $fdtaddr 0x100000
=> adtimg addr $dtboaddr
=> adtimg get dt --index=0 $dtbo0_addr
=> fdt apply $dtbo0_addr

   # Boot Android
=> bootm $loadaddr $loadaddr $fdtaddr
```

## Read the boot partition


Part	Start Sector	Num Sectors	UUID		Type              Secteur  Taille
  1	2990080   	12378112  	00000000-01	0b Boot           0x2DA000          6.3GB - Android
  2	73728     	65536     	00000000-02	06      bootloader
  3	1         	2850816   	00000000-03	05 Extd
  5	139264    	32768     	00000000-05	83   x  env       0x22000    0x8000
  6	172032    	32768     	00000000-06	83   x  boot      0x2A000    0x8000 Android
  7	204800    	1572864   	00000000-07	83      system    0x32000  0x180000 800MB zortrax uboot boots on it
  8	1777664   	32768     	00000000-08	83      misc      0x1B2000   0x8000 Vide
  9	1810432   	65536     	00000000-09	83   x  recovery  0x1BA000  0x10000 Android
 10	1875968   	1048576   	00000000-0a	83      cache     0x1CA000 0x100000 500MB
 11	2924544   	32768     	00000000-0b	83      metadata  0x2CA000   0x8000 Vide
 12	2957312   	32768     	00000000-0c	83      private   0x2D2000   0x8000 Vide


DOS/MBR boot sector (from the first sectors of the EMMC);
partition 1 : ID=0xb, active, start-CHS (0x0,0,0), end-CHS (0x0,0,0), startsector 2990080, 12378112 sectors;
partition 2 : ID=0x6, start-CHS (0x0,0,0), end-CHS (0x0,0,0), startsector 73728, 65536 sectors;
partition 3 : ID=0x5, start-CHS (0x0,0,0), end-CHS (0x0,0,0), startsector 1, 2850816 sectors

```
$ sudo fdisk -l emmcstart.img
Device          Boot   Start      End  Sectors  Size Id Type
emmcstart.img1  *    2990080 15368191 12378112  5,9G  b W95 FAT32   BOOT
emmcstart.img2         73728   139263    65536   32M  6 FAT16       Bmp images and shit
emmcstart.img3             1  2850816  2850816  1,4G  5 Extended
emmcstart.img5        139264   172031    32768   16M 83 Linux       env          uboot env, text
emmcstart.img6        172032   204799    32768   16M 83 Linux       boot         ANDROID
emmcstart.img7        204800  1777663  1572864  768M 83 Linux       system     - android linux
emmcstart.img8       1777664  1810431    32768   16M 83 Linux       misc       - empty
emmcstart.img9       1810432  1875967    65536   32M 83 Linux       recovery     ANDROID
emmcstart.img10      1875968  2924543  1048576  512M 83 Linux       cache
emmcstart.img11      2924544  2957311    32768   16M 83 Linux       metadata   - empty
emmcstart.img12      2957312  2990079    32768   16M 83 Linux       private    - empty
```

The first disk sector is the MBR sector.
Then at sector 16 whe have 64 sectors for sunxi spl (eGON.BT3) signature.
At sector 38192 whe have 1664 sectors for uboot proper.
Then at sector 73728 start the partition 2.

```
mmc read 0x42000000 0x22000 0x8000
fatwrite usb 0:1 0x42000000 env.img 0x1000000

mmc read 0x42000000 0x2A000 0x8000
fatwrite usb 0:1 0x42000000 boot.img 0x1000000

mmc read 0x42000000 0x1BA000 0x10000
fatwrite usb 0:1 0x42000000 recovery.img 0x2000000
```
# Changing the env bootdelay=0 to 2

```
fatload usb 0:1 0x42000000 env.img 0x1000000
mmc write 0x42000000 0x22000 0x8000

mmc read 0x42000000 0x1CA000 0x8000
md 42000000
```

# Saving the bootloader partition
```
mmc dev 1
usb start
mmc read 0x42000000 0x12000 0x10000
fatwrite usb 0:1 0x42000000 bootloader.img 0x2000000
usb stop
```
# Misc
```
mmc read 0x42000000 0x1B2000 0x8000
fatwrite usb 0:1 0x42000000 misc.img 0x1000000
```
# Metadata
```
mmc read 0x42000000 0x2CA000 0x8000
fatwrite usb 0:1 0x42000000 metadata.img 0x1000000
```
# Private
```
mmc read 0x42000000 0x2D2000 0x8000
fatwrite usb 0:1 0x42000000 private.img 0x1000000
```
# system
```
mmc read 0x42000000 0x32000 0x8000
fatwrite usb 0:1 0x42000000 system.img 0x1000000
```
# Read magic.bin from bootloader partition
```
fatload mmc 1:1 0x42000000 magic.bin 0x200
fatwrite usb 0:1 0x42000000 magic.bin 0x200
usb stop
```
# Read the start of the mmc
```
mmc read 0x42000000 0x0000 0x12000
fatwrite usb 0:1 0x42000000 emmcstart.img 0x2400000
```
eGon
uboot

file emmcstart.img 
emmcstart.img: DOS/MBR boot sector; partition 1 : ID=0xb, active, start-CHS (0x0,0,0), end-CHS (0x0,0,0), startsector 2990080, 12378112 sectors; partition 2 : ID=0x6, start-CHS (0x0,0,0), end-CHS (0x0,0,0), startsector 73728, 65536 sectors; partition 3 : ID=0x5, start-CHS (0x0,0,0), end-CHS (0x0,0,0), startsector 1, 2850816 sectors

FILES=$(find .)
for f in $FILES; do xxd "${f}" | grep "d00d" | grep "feed"; done

## If the uboot image contains a lagecy FIT image, i will tru to extract the files

```
dd if=emmcstart.img of=extracted.bin bs=1 skip=$((0x12AB3C4)) count=$((1024*1024))

dumpimage -l  should do something -> no, and the uboot is a raw arm executable.
```

finding the legacy FIT image magic: 0x56190527

# Second FEX binary
Cut part002.bin from 0x78E38 to 0x82AC0 and call this zortraxm200.dtb.
```
dd if=part_0002.bin of=zortraxm200_2_script.bin bs=1 skip=$((0x78E38)) count=$((0x82AC0 - 0x78E38))
```

# First Fex binary
```
dd if=part_0002.bin of=zortraxm200_1_script.bin bs=1 skip=$((0x64E38)) count=$((0x6ed78 - 0x64e38))
```
### So it is no device tree but a compiled FEX file that is given to the kernel.

```
$:~/gitprint/sunxi-tools$ ./sunxi-fexc -v -I bin -O fex ../zortrax/legacyfit/zortraxm200_script.bin zortraxm200
.fex
./sunxi-fexc: from bin:../zortrax/legacyfit/zortraxm200_script.bin to fex:zortraxm200.fex
fexc-bin: ../zortrax/legacyfit/zortraxm200_script.bin: version: 1.2
fexc-bin: ../zortrax/legacyfit/zortraxm200_script.bin: size: 40072 (79 sections), header value: 40072
```

### For VBUS control on USB0 OTG
From uboot, this enables VBUS on the OTG usb port but I never used it in u-boot.
```
gpio clear 225
```
It is from a design mistake somewhere, we can see it in the vendor linux kernel log complaining about vbus detection.

### Boot using the usb stick (very usefull)

```
setenv bootargs root=/dev/sda1 rootwait console=ttyS0,115200  rw
usb start
ext4load usb 0:1 0x42000000  boot/zImage
ext4load usb 0:3 0x43000000  sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```
```
setenv bootargs "root=/dev/sda1 rootwait console=ttyS0,115200"
saveenv
```
```
setenv bootcmd "usb start; ext4load usb 0:1 0x42000000 boot/zImage; ext4load usb 0:3 0x43000000 sun8i-a33-zortrax-m200plus.dtb; bootz 0x42000000 - 0x43000000"
saveenv
```
```
setenv "bootargs root=/dev/sda1 rootwait console=ttyS0,115200"
fatload usb 0:2 0x42000000 zimage
fatload usb 0:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```
```
cd /mnt/
mkdir -p /mnt/ext4disk
mkdir -p /mnt/fat16disk
mount /dev/sda1 ext4disk/
mount /dev/mmcblk2p2 fat16disk
cp ext4disk/u-boot-*.bin fat16disk/
sync
reboot
```
```
fatload mmc 2:2 0x4A000000 u-boot-dtb.bin
go 0x4A000000
```
```
gpio clear 80
```

## The sytem drive on original zortrax emmc is partition 7

```
mount /dev/mmcblk2p7 system
```

## Partition the disk
```
sudo fdisk /dev/mmcblk0
or
sudo parted /dev/mmcblk0
```
The partition table, then uboot, then Armbian.

Or  $armbian-install

```
mount /dev/mmcblk2p1 ext4disk/
```

```
mmc dev 2
setenv devnum 2
saveenv
usb start
ext4load usb 0:3 0x43000000  sun8i-a33-zortrax-m200plus.dtb
run mmc_boot 
```

mais pas de console.

```
mmc dev 2
setenv devnum 2
setenv mmc_bootdev 2
run mmc_boot
```
```
setenv bootargs root=/dev/mmcblk1p1 rootwait console=ttyS0,115200
mmc dev 2
ext4load mmc 2:1 0x42000000  boot/zImage
ext4load mmc 2:1 0x43000000  boot/dtb/sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```

# Read the bootloader from the emmc at sector 16
```
mmc read 0x42000000 0x10 0x10
md 0x42000000
```
## Copy uboot on the emmc

```
dd if=/dev/zero of=/dev/mmcblk2 bs=512 count=40944 seek=16 status=noxfer
dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2 bs=1024 seek=8 conv=fsync

dd  if=Armbian-unofficial_25.11.0-trunk_Lime-a33_bookworm_legacy_6.6.75_minimal.img of=/dev/mmcblk2 bs=4M

dd if=/dev/zero of=/dev/mmcblk2 bs=512 count=40944 seek=16 status=noxfer
dd if=sunxi_zotrax_spl_sect16_64sect.bin of=/dev/mmcblk2 bs=512 seek=16 count=64
dd if=sunxi_zotrax_uboot_sect38192_1664sect.bin of=/dev/mmcblk2 bs=512 seek=38192 count=1664

dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2 bs=512 seek=16 conv=fsync
sync
dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2 bs=1k seek=8
```

A solution would be to use the sunxi bootloader and copy the kernel and the dtb into the FAT16
partition and change env variables to boot using it.
Reformat the main partition as ext4, and copy Armbian legacy rootfs into the main partition.

### Trying to use the original image

Restore the image
```
cd /mnt/
mount /dev/sda3 ext4disk/
cd ext4disk/
dd if=emmc.image of=/dev/mmcblk2 bs=4M conv=fsync
```
Save the env image
```
dd if=/dev/mmcblk2p5 of=sunxi_env.img bs=1k count=4
```
Change the env image
```
dd if=env.img of=/dev/mmcblk2p5
```

Mout the FAT16 partition and copy the kernel and dtb
```
mkdir -p fat16disk
mount /dev/mmcblk2p2 fat16disk
cp boot/zImage fat16disk/
cp boot/dtb/sun8i-a33-zortrax-m200plus.dtb
umount fat16disk
```
```
cd /mnt/
mount /dev/sda1 ext4disk/
mount /dev/mmcblk2p2 fat16disk
cp ext4disk/u-boot-*.bin fat16disk/
```

cd
Delete the system partition and recreate it as ext4 (Do not do that! Keep it Zortrax and use a USB drive instead, theese lines are development notes).
```
parted /dev/mmcblk2
rm 3
mkpart primary ext4 2990080s 12378112s
quit
mkfs.ext4 /dev/mmcblk2p1
mount /dev/mmcblk2p1 ext4disk/
cd ext4disk/
tar xvf /moredata/armbian/Armbian-unofficial_25.11.0-trunk_Lime-a33_bookworm_legacy_6.6.75_minimal.img.tar.xz
umount ext4disk
```
```
using fdisk
d
1
n
p
1
2990080
15269887
w
```
```
mkfs.ext4 /dev/mmcblk2p1
blkid /dev/mmcblk2p1
```
```
mount -o loop,offset=4194304 Armbian-unofficial_25.11.0-trunk_Lime-a33_bookworm_legacy_6.6.75_minimal.img armbian_rootfs
mount /dev/mmcblk2p1 emmcarmbian/
rsync -aAXv armbian_rootfs/ emmcarmbian/

reboot
```

## Modify the uboot env to boot from mmcblk2p2
## Change bootcmd to load from mmc 2:2

```
setenv bootcmd 'mmc dev 2; fatload mmc 2:2 0x42000000 zImage; fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb; bootz 0x42000000 - 0x43000000'
saveenv
```

## Change bootargs to root=/dev/mmcblk2p1

```
setenv bootargs 'root=/dev/mmcblk2p1 rootwait console=ttyS0,115200'
saveenv
reboot
```

```
fatls mmc 2:2
mmc dev 2
setenv bootargs root=/dev/mmcblk2p1 rootwait console=ttyS0,115200
fatload mmc 2:2 0x42000000 zimage
fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```
OK boots from emmc if from uboot in FEL mode, a bit weird. Read only file system.


gives Wrong Image Format for bootm command
ERROR: can't get kernel image!

### Convet zImage to uImage

```
mkimage -A arm -O linux -T kernel -C none -a 0x40008000 -e 0x40008000 -n "Linux Kernel" -d zImage uImage
```

### Boot

```
setenv bootargs root=/dev/mmcblk0p7 rootwait console=ttyS0,115200  init=/init
fatload mmc 2:2 0x42000000 uimage
fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootm 0x42000000 - 0x43000000
```
Boots and hangs?????

# Chainload uboot from the older one

```
fatload mmc 2:2 0x42000000 u-boot-sunxi-with-spl.bin 596216
go 0x42000000
```
fail
```
fatload mmc 2:2 0x40000000 u-boot-sunxi-with-spl.bin 596216
go 0x40000000
```
fail
```
fatload mmc 2:2 0x4A000000 u-boot-sunxi-with-spl.bin 596216
go 0x4A000000
```
semi fail from the sunxi uboot
..P
U-Boot SPL 2026.01-rc2-gd9b92b4449b3-dirty (Nov 13 2025 - 19:01:00 +0100)
DRAM:

Maybe by specifying the DRAM settings



from last uboot
```
fatload mmc 2:2 0x43000000 u-boot-sunxi-with-spl.bin 596216
go 0x43000000
```
fail

```
fatload mmc 2:2 0x4A000000 u-boot.bin 563384
go 0x4A000000
```
Ok Chainload!!!!!! :o
But MMC is messed up

use u-boot-dtb.bin
```
fatload mmc 2:2 0x4A000000 u-boot-dtb.bin
go 0x4A000000
```
Ok Chainload.
But MMC is still messed up

using u-boot-dtb.bin with EMMC=2
```
fatload mmc 2:2 0x4A000000 u-boot-dtb.bin
go 0x4A000000
```
NOPE

using u-boot-dtb.bin with EMMC=2 from usb
```
usb start
ext4load usb 0:3 0x4A000000  u-boot-dtb.bin
go 0x4A000000
```
MMC ok but at 1 instead of 2

using u-boot-dtb.bin with only CONFIG_SPL_MMC from usb
```
usb start
ext4load usb 0:3 0x4A000000  u-boot-dtb.bin
go 0x4A000000
```
OK, with MMC 2

```
setenv vendorbootcmd "run setargs_mmc boot_normal"
setenv bootcmd "fatload mmc 2:2 0x4A000000 u-boot-dtb.bin; go 0x4A000000"
saveenv
```

```
fatls mmc 2:2
setenv bootargs root=/dev/mmcblk2p1 rootwait console=ttyS0,115200 
saveenv
fatload mmc 2:2 0x42000000 zimage
fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```
FAILED] Failed to start systemd-re…ount Root and Kernel File Systems.
fstab problem, duck duck go > OpenAI GPT4.1
To get the UUID of the root partition:
blkid /dev/mmcblk2p1
UUID="33b43a40-9415-4958-b0be-6c5a1e17e26d"

Now from emmc with fixed fstab
```
fatload mmc 2:2 0x4A000000 u-boot-dtb.bin
go 0x4A000000
setenv bootargs root=/dev/mmcblk2p1 rootwait console=ttyS0,115200
```
Fucked up emmc

From usb fel mode with fixed fstab
Much better.

Only needs emmc detection from a chainloaded uboot. It works from fel mode but not when chainloading.
From EMMC with CONFIG_DM_RESET
nope

Adding mmc things in dts0
ofnode_read_u32_index: vmmc-supply: 0x11 (17)
ofnode_read_u32_index: vqmmc-supply: 0x11 (17)
Card did not respond to voltage select! : -110
mmc_init: -95, time 22
NOPE


## Booting by chain loading from the EMMC FAT16 part, and then loading not from the emmc but from the fat16 partition on usb
```
fatload mmc 2:2 0x4A000000 u-boot-dtb.bin
go 0x4A000000
setenv bootargs root=/dev/mmcblk2p1 rootwait console=ttyS0,115200
fatload usb 0:2 0x42000000 zimage
fatload usb 0:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```

kernel panic, worked at some point -> because Android boot overwrote the partition
But boot fine now.

## Fatload from the emmc using sunxiboot, and call bootz from the chainloaded mainline uboot
```
fatload mmc 2:2 0x42000000 zimage
fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
fatload mmc 2:2 0x4A000000 u-boot-dtb.bin
go 0x4A000000
setenv bootargs root=/dev/mmcblk2p1 rootwait console=ttyS0,115200
bootz 0x42000000 - 0x43000000
```
Nope

using u-boot.img:
```
fatload mmc 2:2 0x4A000000 u-boot.img
bootm 0x4A000000
```
Wrong format, goes back to prompt

## Booting kernel from Legacy Image at 4a000000 ...

   Image Name:   U-Boot 2026.01-rc2-gd9b92b4449b3
   Image Type:   ARM U-Boot Firmware (uncompressed)
   Data Size:    563384 Bytes = 550.2 KiB
   Load Address: 4a000000
   Entry Point:  4a000000
   Verifying Checksum ... OK
Wrong Image Type for bootm command
ERROR: can't get kernel image!

use u-boot-dtb.img
```
fatload mmc 2:2 0x4A000000 u-boot-dtb.img
bootm 0x4A000000
```

## Booting kernel from Legacy Image at 4a000000 ...

   Image Name:   U-Boot 2026.01-rc2-gd9b92b4449b3
   Image Type:   ARM U-Boot Firmware (uncompressed)
   Data Size:    563384 Bytes = 550.2 KiB
   Load Address: 4a000000
   Entry Point:  4a000000
   Verifying Checksum ... OK
Wrong Image Type for bootm command
ERROR: can't get kernel image!

sunxi#fatload mmc 2:2 0x4A000000 uimage 
sunxi#bootm 0x4A000000

## Booting kernel from Legacy Image at 4a000000 ...

   Image Name:   Linux Kernel
   Image Type:   ARM Linux Kernel Image (uncompressed)
   Data Size:    10748704 Bytes = 10.3 MiB
   Load Address: 40008000
   Entry Point:  40008000
   Verifying Checksum ... OK
   Loading Kernel Image ... OK
OK
para err in disp_ioctl, cmd = 0xa,screen id = 1
[    118.337][mmc]: mmc exit start
[    118.341][mmc]: mmc 2 2xmode config clk
[    118.351][mmc]: mmc 2 2xmode config clk
[    118.363][mmc]: mmc 2 cmd 8 timeout, err 100
[    118.368][mmc]: smc 2 err, cmd 8,  RTO
[    118.373][mmc]: mmc 2 mmc cmd 8 err 0x00000100
[    118.377][mmc]: mmc send if cond failed
[    118.381][mmc]: mmc 2 cmd 55 timeout, err 100
[    118.385][mmc]: smc 2 err, cmd 55,  RTO
[    118.391][mmc]: mmc 2 mmc cmd 55 err 0x00000100
[    118.395][mmc]: send app cmd failed
[    118.428][mmc]: mmc 2 exit ok
[    118.431]
Starting kernel ...

```
setenv bootargs root=/dev/mmcblk1p1 rootwait console=ttyS0,115200 
fatload mmc 2:2 0x4A000000 uimage
fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootm 0x4A000000 - 0x43000000
```
NOPE

```
setenv bootargs root=/dev/mmcblk1p1 rootwait console=ttyS0,115200 
fatload mmc 2:2 0x4A000000 uimage
fatload mmc 2:2 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootm 0x4A000000 - 0x43000000
```
NOPE

go 0x42000000
Hangs and reboots

## DO NOT TRY AT HOME

!!!!!!!!!!!!!!! BRICKs the board !!!!!!!!!!!!!!!!!!!!
-------------------------- THIS BREAKS THE BOARD, NO FEL MODE AFTER THAT!!!!  ----------------------------
Wipe it:
echo 0 > /sys/block/mmcblk2boot0/force_ro
echo 0 > /sys/block/mmcblk2boot1/force_ro
dd if=/dev/zero of=/dev/mmcblk2boot0 bs=1k count=4095 seek=1 status=noxfer
dd if=/dev/zero of=/dev/mmcblk2boot1 bs=1k count=4095 seek=1 status=noxfer

Now flash new u-boot bootloader into boot partitions:
dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2boot0 bs=1024 seek=8 status=noxfer
dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2boot1 bs=1024 seek=8 status=noxfer
Because it is loaded but hangs and does not help with anything but bricks the board
since the FEL mode is not available anymore.
!!!!!!!!!!!!!!! BRICKs the board !!!!!!!!!!!!!!!!!!!!

After that your board is a brick, you need to replace the EMMC with a new programmed one.
------------------------------------------------------------------------------------------------

```
dd if=/dev/mmcblk2 bs=512 skip=16 count=1 | hexdump -C
dd if=/dev/mmcblk2 bs=512 skip=38192 count=1 | hexdump -C
```
```
dd if=sunxi_zotrax_spl_sect16_64sect.bin of=/dev/mmcblk2 bs=512 seek=16 count=64
```

## Uboot SPL

```
dd if=/dev/mmcblk2 bs=512 skip=16 count=1 | hexdump -C
```
## U-Boot SPL area for mainline u-boot 0x8000. The sunxi one is much further away.

```
dd if=/dev/mmcblk2 bs=512 skip=80 count=1 | hexdump -C
```

The boot0 code is not able to load this code. It says:
```
[mmc]: ***SD/MMC 2 init OK!!!***
sdcard 2 init ok
ERROR! NOT find the head of uboot.
```

Trying to write uboot.img at the same place as the original zortrax uboot
dd if=u-boot.img of=/dev/mmcblk2 bs=512 seek=38192 conv=fsync
Does not work


# Read the sunxi boot spl area

```
mmc dev 2
mmc read 0x42000000 0x10 0x10
md 0x42000000
```
0x1376000 - 0x12A6000 = D0000
```
dd if=/dev/zero of=/dev/mmcblk2 bs=512 count=1664 seek=38192 status=noxfer
dd if=u-boot-dtb.bin of=/dev/mmcblk2 bs=512 seek=38192 count=1101
dd if=/dev/mmcblk2 bs=512 skip=38192 count=1 | hexdump -C
dd if=legacy_3p4/sunxi_zotrax_uboot_sect38192_1664sect.bin of=/dev/mmcblk2 bs=512 seek=38192 count=1664
dd if=/dev/mmcblk2 bs=512 skip=38192 count=1 | hexdump -C
```

$:~/moredata/emmc/Zortrax_OK$ /usr/local/bin/sunxi-fw info -v emmc.image 
@   0: mbr: DOS MBR
	part 1: type: 0B, offset:  2990080 sectors, size: 12378112 sectors
	part 2: type: 06, offset:    73728 sectors, size:    65536 sectors
	part 3: type: 05, offset:        1 sectors, size:  2850816 sectors
	first partition starts at 0 KB
@   1: mbr: DOS MBR
	part 1: type: 83, offset:   139263 sectors, size:    32768 sectors
	part 2: type: 05, offset:        1 sectors, size:    32768 sectors
	first partition starts at 0 KB
@   2: mbr: DOS MBR
	part 1: type: 83, offset:   172030 sectors, size:    32768 sectors
	part 2: type: 05, offset:        2 sectors, size:    32768 sectors
	first partition starts at 1 KB
@   3: mbr: DOS MBR
	part 1: type: 83, offset:   204797 sectors, size:  1572864 sectors
	part 2: type: 05, offset:        3 sectors, size:  1572864 sectors
	first partition starts at 1 KB
@   4: mbr: DOS MBR
	part 1: type: 83, offset:  1777660 sectors, size:    32768 sectors
	part 2: type: 05, offset:        4 sectors, size:    32768 sectors
	first partition starts at 2 KB
@   5: mbr: DOS MBR
	part 1: type: 83, offset:  1810427 sectors, size:    65536 sectors
	part 2: type: 05, offset:        5 sectors, size:    65536 sectors
	first partition starts at 2 KB
@   6: mbr: DOS MBR
	part 1: type: 83, offset:  1875962 sectors, size:  1048576 sectors
	part 2: type: 05, offset:        6 sectors, size:  1048576 sectors
	first partition starts at 3 KB
@   7: mbr: DOS MBR
	part 1: type: 83, offset:  2924537 sectors, size:    32768 sectors
	part 2: type: 05, offset:        7 sectors, size:    32768 sectors
	first partition starts at 3 KB
@   8: mbr: DOS MBR
	part 1: type: 83, offset:  2957304 sectors, size:    32768 sectors
	first partition starts at 1478652 KB
@   9: mbr: DOS MBR
	part 1: type: 83, offset:  6135799 sectors, size:     1024 sectors
	part 2: type: 05, offset:        9 sectors, size:     1024 sectors
	first partition starts at 4 KB
@  10: mbr: DOS MBR
	part 1: type: 83, offset:  6136822 sectors, size:    31744 sectors
	part 2: type: 05, offset:       10 sectors, size:    31744 sectors
	first partition starts at 5 KB
@  11: mbr: DOS MBR
	part 1: type: 83, offset:  6168565 sectors, size:   163840 sectors
	part 2: type: 05, offset:       11 sectors, size:   163840 sectors
	first partition starts at 5 KB
@  12: mbr: DOS MBR
	part 1: type: 83, offset:  6332404 sectors, size:    32768 sectors
	first partition starts at 3166202 KB
@  16: boot0: Allwinner boot0
	size: 32768 bytes
	eGON checksum matches: 0x67a6f033
	DRAM parameters:           A64        H616        A133   
		DRAM clock  :         552         552         552
		DRAM type   :         0x3         0x3         0x3
		ZQ value    :      0x3bbb           -           -
		ODT enabled :         0x1      0x1000           -
		DX ODT      :           -      0x3bbb      0x3bbb
		DX DRI      :           -         0x1         0x1
		CA DRI      :           -  0x10f20400  0x10f20400
		PARA0       :           -           -      0x1000
		PARA1       :  0x10f20400      0x1c70      0x1c70
		PARA2       :      0x1000        0x40        0x40
		MR0         :      0x1c70        0x18        0x18
		MR1         :        0x40           0           0
		MR2         :        0x18    0x47214f    0x47214f
		MR3         :           0   0x1c2294b   0x1c2294b
		MR4         :           -     0x61043     0x61043
		TPR0        :    0x47214f        0xa8           0
		TPR1        :   0x1c2294b     0x10901           0
		TPR2        :     0x61043           0        0xa8
		TPR3        :           0           0     0x10901
		TPR12       :        0xa8           0           0
		TPR13       :     0x10901           0           0


### Sunxi boot roms release dates on th eprinter

mmcblk2boot0 primary bootloader:
[mmc]: mmc driver ver 2014-07-07 16:54

uboot:
U-Boot 2011.09-rc1 (Mar 21 2017 - 01:10:46) Allwinner Technology 

2011.09-rc1 compiled the 21 Mar 2017

