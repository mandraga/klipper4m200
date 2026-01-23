## Connect to the serial console

microcom -p /dev/ttyUSB0 -s 115200

## Linux boot

Kernel command line: console=ttyS0,115200

root=/dev/mmcblk0p7 
partitions=
    bootloader@mmcblk0p2
    env@mmcblk0p5
    boot@mmcblk0p6
    system@mmcblk0p7
    misc@mmcblk0p8
    recovery@mmcblk0p9
    cache@mmcblk0p10
    metadata@mmcblk0p11
    private@mmcblk0p12
    UDISK@mmcblk0p1 

[    0.000000] Memory: 1024MB = 1024MB total
[    0.000000] Memory: 845372k/845372k available, 203204k reserved, 90112K highmem

512 x 16bits = 1GB

## MMC

[mmc]: ***Try MMC card 2***
[mmc]: MMC ver 5.0
[mmc]: SD/MMC Card: 4bit, capacity: 7456MB
[mmc]: vendor: Man 0x00110100 Snr 0x00f15daa
[mmc]: product: 008G7
[mmc]: revision: 3.0
[mmc]: ***SD/MMC 2 init OK!!!***

## FEL mode

Enter FEL mode by pressing 2 on the serial console.
https://linux-sunxi.org/Retrieving_device_information
https://linux-sunxi.org/FEL/USBBoot

Press '2' on the console during boot to get to FEL mode on USB0. USB0 will be in device mode without VBUS (5V).

## Enter ADB mode

Press the home button when powering on, the board boots in ADB mode.
However no debug interface is given.

## LCD screen

Resolution 480 x 800
