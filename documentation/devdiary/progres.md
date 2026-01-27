## Access

- Console is on ART0 -> pressing '2' at start enables FEL mode
- USB0 on P4 is the OTG port -> it allows FEL mode procotol

## Andoid debug

Pressing HOME at start loads the Android Debug Mode port, however, the USB device
is not enabled. So no  ADB.

## Reading the content

The FEL mode is the protected mmcblk2Boot0 EMMC partition.
It allows to load a FES image, do not touch it. Any error will brick the device (unless you can solder a new programmed EMMC).

On the Zortrax, the Allwinner bootloader first looks for FEL mode, then locates SDcard 2 (non cxistent) and the EMMC2.
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
Sinlinx SinA33	                        sun8i-a33-sinlinx-sina33.dtb              1GB DDR3 、4GB EMMC

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
eraly jump fel

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
press 'e' to start FEL mode
```

In another terminal:
```
sudo ./sunxi-fel uboot u-boot-sunxi-with-spl.bin

```

This loads u-boot on the system.