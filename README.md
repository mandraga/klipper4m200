# klipper4m200

A replacement firmare for the Zortrax M200+ 3D printer.

This firmware is based on Debian/Armbian and allows to install klipper on the m200+.
? There is more or less no comming back.

## Access the internal single board comuter

You need a serial port on the UART0 J5 connector.
Plug a special USB-A to USB-A cable with no 5V connexion to be able to use the FEL mode.

At boot, press 2 on the serial console, it will start in FEL mode.
You should see this on "dmesg -w":
```
ID 1f3a:efe8 Allwinner Technology sunxi SoC OTG connector in FEL/flashing mode
```
Then, using sunxi-tools, you can upload a uboot image like this:
```
sunxi-tools$ sudo ./sunxi-fel uboot u-boot-sunxi-with-spl-4m200.bin
```
It starts uboot and you can browse files and dump data.

Then you can use this uboot prompt to save or restore data from a usb stick.
Because uboot data transfers are no more than 16MB on this machine, we must do some workaroud.
A set of scripts allows you to:
- dump the original disk image on the usb stick.
- flash the device with a custom image, like a Debian.

## Download the image



## At first boot, it will flash the microcontroller and start Klipper.


