## Access

- Console is on ART0 -> pressing '2' at start enables FEL mode
- USB0 on P4 is the OTG port -> it allows FEL mode procotol

## Andoid debug

Pressing HOME at start loads the Android Debug Mode port, however, the USB device
is not enabled. So no  ADB.

## Reading the content

The FEL mode is the protected mmcblk2Boot0 EMMC partition.
It allows to load a FES image, do not touch it. Any error will brick the device (unless you can solder a new programmed EMMC).

On the Zortrax, the Allwinner bootloader first looks for FEL mode, then locates SDcard 0 (non existent) and the EMMC2.
Then it loads UBoot.
Uboot cannot be stopped to use it to scan the partitions as his bootdelay is 0.
Uboot loads the linux Kernel.

On FEL mode from a remote console, we can only read the RAM.

## Machine

Machine: sun8i
Linux version 3.4.39 

### FEL 

./sunxi-fel -l
USB device 003:011   Allwinner A33     0461872a:890c2042:ab98b7e7:00000000

### A33 sun8i machines

#### Tablets
Allwinner GA10H Quad Core Tablet (v1.1) sun8i-a33-ga10h-v1.1.dtb
ET Q8 Quad Core Tablet (v1.6)	        sun8i-a33-et-q8-v1.6.dtb
INet-D978 Rev 02	                    sun8i-a33-inet-d978-rev2.dtb
Q8 A33 Tablet	                        sun8i-a33-ippo-q8h-v1.2.dtb
Q8 A33 Tablet	                        sun8i-a33-q8-tablet.dtb

#### Single board computers (very similar)
Olimex A33-OLinuXino	                sun8i-a33-olinuxino.dtb
Sinlinx SinA33	                        sun8i-a33-sinlinx-sina33.dtb    1GB DDR3 、4GB EMMC

## FEX files

A usefull commented fex file is here:
https://github.com/linux-sunxi/sunxi-boards/blob/master/sys_config/a33/sinlinx_sina33.fex

## Compiling a Uboot SPL

I try to compile uboot with A33-OLinuXino_defconfig and use sun8i-a33-olinuxino.dtb

```
make CROSS_COMPILE=arm-linux-gnueabihf- A33-OLinuXino_defconfig
make CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)

sudo ./sunxi-fel uboot u-boot-sunxi-with-spl.bin
```


```
early jump fel

U-Boot SPL 2025.10-rc5 (Oct 19 2025 - 13:51:27 +0200)
DRAM: 1024 MiB
Trying to boot from FEL


U-Boot 2025.10-rc5 (Oct 19 2025 - 13:51:27 +0200) Allwinner Technology

CPU:   Allwinner A33 (SUN8I 1667)
Model: Olimex A33-OLinuXino
DRAM:  1 GiB
Core:  60 devices, 21 uclasses, devicetree: separate
WDT:   Not starting watchdog@1c20ca0
MMC:   
```

Hangs at MMC, so not the proper MMC adress.
Recreating a temporary dts, the MMC is not found, hence it will be a problem.

UBoot for sinlinx A33 does not help

## EMMC ports

SDIO0 is used for the unpopulated U5 wifi chip.
SDIO1 is also left hanging.
And the chip uses and only boots on SDIO2/MMC2.
- In 4 bit mode for UBoot, and double speed mode for linux.

### PMIC

axp22_board axp22_board: AXP (CHIP ID: 0x06) detected
axp22_dcdc1: 1600 <--> 3400 mV at 3000 mV 
axp22_dcdc2: 600 <--> 1540 mV at 1100 mV 
axp22_dcdc3: 600 <--> 1860 mV at 1200 mV 
axp22_dcdc4: 600 <--> 1540 mV at 1100 mV 
axp22_dcdc5: 1000 <--> 2550 mV at 1500 mV 
axp22_rtc: 3000 mV 
axp22_aldo1: 700 <--> 3300 mV at 3000 mV 
axp22_aldo2: 700 <--> 3300 mV at 2500 mV 
axp22_aldo3: 700 <--> 3300 mV at 3000 mV 
axp22_dldo1: 700 <--> 3300 mV at 700 mV 
axp22_dldo2: 700 <--> 3300 mV at 1800 mV 
axp22_dldo3: 700 <--> 3300 mV at 700 mV 
axp22_dldo4: 700 <--> 3300 mV at 700 mV 
axp22_eldo1: 700 <--> 3300 mV at 1800 mV 
axp22_eldo1: supplied by axp22_dcdc1
axp22_eldo2: 700 <--> 3300 mV at 700 mV 
axp22_eldo2: supplied by axp22_dcdc1
axp22_eldo3: 700 <--> 3300 mV at 700 mV 
axp22_eldo3: supplied by axp22_dcdc1
axp22_dc5ldo: 700 <--> 1400 mV at 1100 mV 
axp22_ldoio0: 700 <--> 3300 mV at 3800 mV 
axp22_ldoio1: 700 <--> 3300 mV at 3800 mV 
axp22_dc1sw: at 700 mV 
axp22_dc1sw: supplied by axp22_dcdc1
input: axp22-supplyer as /devices/platform/axp22_board/axp22-supplyer.20/input/input0


### Boot log during Uboot

```
[      1.444][mmc]: ---------------mmc->clock 50000000-----------
[      1.449][mmc]: ---------------mmc->bus_width 4--------------
[      1.455][mmc]: SD/MMC Card: 4bit, capacity: 7456MB
[      1.460][mmc]: boot0 capacity: 4000KB,boot1 capacity: 4000KB
```

### DRAM log

```
[    0.566478] [ddrfreq] dram_clk value is 552
[    0.566489] [ddrfreq] sunxi_ddrfreq_max=552000
[    0.566499] [ddrfreq] dram_type value is 3
[    0.566509] [ddrfreq] dram_zq value is 0x3bbb
[    0.566519] [ddrfreq] dram_odt_en value is 1
[    0.566530] [ddrfreq] dram_para1 value is 0x10f20400
[    0.566541] [ddrfreq] dram_para2 value is 0x1000
[    0.566551] [ddrfreq] dram_mr0 value is 0x1c70
[    0.566561] [ddrfreq] dram_mr1 value is 0x40
[    0.566571] [ddrfreq] dram_mr2 value is 0x18
[    0.566581] [ddrfreq] dram_mr3 value is 0x0
[    0.566591] [ddrfreq] dram_tpr0 value is 0x47214f
[    0.566601] [ddrfreq] dram_tpr1 value is 0x1c2294b
[    0.566612] [ddrfreq] dram_tpr2 value is 0x61043
[    0.566622] [ddrfreq] dram_tpr3 value is 0x0
[    0.566632] [ddrfreq] dram_tpr4 value is 0x0
[    0.566642] [ddrfreq] dram_tpr5 value is 0x0
[    0.566652] [ddrfreq] dram_tpr6 value is 0x0
[    0.566662] [ddrfreq] dram_tpr7 value is 0x0
[    0.566672] [ddrfreq] dram_tpr8 value is 0x0
[    0.566682] [ddrfreq] dram_tpr9 value is 0x0
[    0.566692] [ddrfreq] dram_tpr10 value is 0x0
[    0.566702] [ddrfreq] dram_tpr11 value is 0x0
[    0.566712] [ddrfreq] dram_tpr12 value is 168
[    0.566721] [ddrfreq] sunxi_ddrfreq_min=168000
[    0.566732] [ddrfreq] dram_tpr13 value is 0x10901
```

## Loggin in

```
microcom -p /dev/ttyUSB0 -s 115200
press '2' to start FEL mode
```

In another terminal:
```
sudo ./sunxi-fel uboot u-boot-sunxi-with-spl.bin

```

This loads u-boot on the system.


### After some progress on another A33 system I understood a fex things about my u-boot failures

I need to specify the power lines and the RAM parameters according to the fex files, otherwise it does not boot directly on the machine. I added the parameters and reflashed with confidence the boot sector on the EMMC thinking that I would reinstall from fel mode if it did not work.

IT WAS A MISTAKE! Yes u-boot started, but could not find any EMMC and I COULD NOT ENTER the FEL mode
again! Below is the log of what I did from the armbian console:

```
patrick@lime-a33:~$ sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblk2 bs=1024 seek=8 DO NOT DO THAT!!!
[sudo] Mot de passe de patrick : 
[  433.610039]  mmcblk2: p1 p2 p3 < p5 p6 p7 p8 p9 p10 p11 p12 >
[  433.615911] mmcblk2: p1 size 12378112 extends beyond EOD, truncated
592+1 enregistrements lus
592+1 enregistrements écrits
606832 octets (607 kB, 593 KiB) copiés, 0,0985589 s, 6,2 MB/s
patrick@lime-a33:~$ sync
```

The log shows:
```
�
U-Boot SPL 2026.01-rc2-g5b13f84438d6-dirty (Mar 16 2026 - 20:16:29 +0100)
DRAM: 1024 MiB
Trying to boot from MMC2
MMC Device 1 not found
spl: could not find mmc device 1. error: -19
Error: -19
SPL: Unsupported Boot Device!
SPL: failed to boot from all boot devices
### ERROR ### Please RESET the board ###
```

But I assumed would show:
```
HELLO! BOOT0 is starting!
boot0 version : 3.1.0
reg_addr 0x01f00100 =0x00000000
reg_addr 0x01f00104 =0x00000000
reg_addr 0x01f00108 =0x00000000
reg_addr 0x01f0010c =0x00000000
reg_addr 0x01f00110 =0x00000000
reg_addr 0x01f00114 =0x00000000
DRAM DRIVE INFO: V1.5
DRAM CLK =552 MHZ
DRAM simple test OK.
dram size =1024
card boot number = 2
card no is 2
sdcard 2 line count 0
[mmc]: mmc driver ver 2014-07-07 16:54
[mmc]: ***Try SD card 2***
[mmc]: mmc 2 cmd 8 timeout, err 0x00000100
[mmc]: mmc 2 cmd 8 err 0x00000100
[mmc]: mmc 2 send if cond failed
[mmc]: mmc 2 cmd 55 timeout, err 0x00000100
[mmc]: mmc 2 cmd 55 err 0x00000100
[mmc]: mmc 2 send app cmd failed
[mmc]: ***Try MMC card 2***
[mmc]: MMC ver 5.0
[mmc]: SD/MMC Card: 4bit, capacity: 7456MB
[mmc]: vendor: Man 0x00150100 Snr 0x005c1dfe
[mmc]: product: 8WPD3
[mmc]: revision: 5.2
[mmc]: ***SD/MMC 2 init OK!!!***
sdcard 2 init ok
The size of uboot is 0x000bc000.
sum=0xb3d23440
src_sum=0xb3d23440
set_mmc_para,sdly 50M 0
set_mmc_para,sdly 25M 0
Succeed in loading uboot from sdmmc flash.
Ready to disable icache.
Jump to secend Boot.
[      0.332]

U-Boot SPL 2026.01-rc2-g5b13f84438d6-dirty (Mar 16 2026 - 20:16:29 +0100)
DRAM: 1024 MiB
Trying to boot from MMC2
MMC Device 1 not found
spl: could not find mmc device 1. error: -19
Error: -19
SPL: Unsupported Boot Device!
SPL: failed to boot from all boot devices
### ERROR ### Please RESET the board ###
```

The primary bootloader does not show up. My u-boot starts but it does not find the EMMC and fails without FEL mode.
Until now, unless the one time I destroyed the Boot0 sector, I was able to boot in FEL mode.

### I understand:

This is the second time I destroyed my boot0 sectors.
Because they can be seen as separate partitions in liunx, I assumed that boot0 was outside of the EMMC. But in fact, the first sectors of the flash are boot0. Because it is a new soldered version of the EMMC, I must have forgotten to lock them back when programming it.
My attemps to boot from the SD card instead of soldering a new chip failed because I had no proper memory/power rails setup in my u-boot. Only chainloading worked. The memory was then intialised because of the previous initialisation from the stock u-boot. My memory setup was wrong, and I could not write my bootloader because the boot sectors where protected.
Now, I will be able to have a proper u-boot chainloaded from the first one, but before changing the EMMC, I will retry my SD card mod and use fel-sdboot.sunxi on an SD card to enable FEL mode.
And until I can get FEL mode from my u-boot, I will chainload.

So at 8K we have the primary bootloader in a protected area. And after 0xA000 we have the secondary bootloader which is a u-boot.bin type.
On my two systems this secondary boot was located at 0x12A6000.

Turns out my new EMMC seemed not fully soldered such as it does not enable the 8bit interface. The Boot0 sectors shows up but not the other data.
-> in fact the soldering whent wel it seems, and the problem is already seen on the original firmware which boots fine:
* The fex file shows that emmc2 is disabled, I assume that the sunxi u-boot source code has been patched for a custom emmc init.
* In the stock boot, we see a clock reduced to 50Mhz and only 4 bits used instead of 8.
Once this is applied in the device tree, and once u-boot is compiled with mmc mode command, we can list the emmc partitions in u-boot and boot over emmc and not USB.
We can also use the device as a mass storage system.

https://docs.u-boot.org/en/latest/usage/cmd/mmc.html

In u-boot, mount the mmc2 in sd card speed mode (50Mhz):
```
=> mmc dev 1 0 1
=> mmc part
=> mmc rescan
=> ums 0 mmc 1
```

The parts apear in linux, we can dump the EMMC or configure the boot process:

Using the m200 as a mass storage (not from Armbian):
```
sudo dd if=sunxi_env.img of=/dev/sde5
```

However it is riskied to do things like this the from Armbian console. You will have soon destroyed one of your drives instead of programmed the m200.

## Installing on the flash.

We cannot at all replace Boot0, too risky. We can flash the stage two in place of the sunxi one an see what is going on.
Flash my u-boot proper to 0x12A6000 to see if it works.

From Armbian:
```
sudo dd if=u-boot-dtb.bin of=/dev/mmcblk2 bs=512 seek=38192
```

This does not work, stage 1 reports this:

```
[mmc]: ***SD/MMC 2 init OK!!!***
sdcard 2 init ok
ERROR! NOT find the head of uboot.
Ready to disable icache.
Jump to Fel.
```

But the chainloaded uboot would now have access to the EMMC.

No chainload works with EMMC, loading linux from th esunxi bootloader fials, event with their zImage.
The system is so closed, that I do not want to change Boot0. And I replaced my EMMC twice so enoug.
However the MMC performances are great compared to a Sandisk Cruzer blade. I made my tests for a while on a USB3 memory stick. It is gread but Armbian gives a warning "slow drive" with the Sandisk.
I have found a way to encasulate u-boot-dtb.bin in the sunxi format.
I will make a app to encapsulate mainline u-boot in sunxi format.
It wil be called sunxisecondstagetool.

#$ sunxisecondstagetool u-boot-dtb.bin

## And rebrick!

I managed to overwrite again the EMMC! Using the wrong command, and the write protection failed prior to that. While I double checked it did not work.

### Boor protection:

```
cat /sys/block/mmcblk2boot1/ro
cat /sys/block/mmcblk2boot1/force_ro
and
sudo mmc extcsd read /dev/mmcblk2 | grep -E 'BOOT_WP|BOOT_CONFIG_PROT|PARTITION_CONFIG|USER_WP'
```

* BOOT_WP: 0x00 → no boot partition write protection set
* nonzero BOOT_WP → temporary or permanent boot write protection is enabled


### A33 core schematics.

I found some schematis for the A33 Core board (https://github.com/xianxuhappy/A33_M2).
So, the only positive thing, is that if you brick the board, there is a sdio1 on the wifi chip. So maybe after removing the emmc, the chip will boot on SDC1.

## Debrik and boot using sunxi

I managed to debrik my third printer using a capacitor.
Then I managed to pack uboot proper into a structure understandable to the legacy sunxi uboot0 (see submodule).
But while I had the emmc working on u-boot proper, linux failed to start as expected.
It was hanging after some good progress on the kernel launch.

So back to chainloading, but I put the rootfs on the emmc and it is much faster than the USB stick.
```
setenv bootargs root=/dev/mmcblk2p1 rootwait console=ttyS0,115200 rw
ext4load usb 0:2 0x42000000 boot/zImage
ext4load usb 0:1 0x43000000 sun8i-a33-zortrax-m200plus.dtb
bootz 0x42000000 - 0x43000000
```

Now I have the screen init problem again. The fix is to unload and load the driver.

```
sudo modprobe -r panel-mtf0397swi
sudo modprobe panel-mtf0397swi
```
I added it in a service. And tryed a ton of things, but I guess it's the chainloading.
If unbricking through a capacitor works for boot0, I will try again u-boot with SPL on Boot0. And FEL mode would be achieved by chainloading the FEL image for SD card.

## Changing the machine name

sudo hostnamectl set-hostname M200Plus_A33
sudo nano /etc/hosts <- replace the names

