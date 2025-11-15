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
sunxi-tools$ sudo ./sunxi-fel uboot u-boot-sunxi-with-spl.bin
```
It starts uboot and you can browse files and dump data.

Then you can use this uboot prompt to boot Armbian from the usb stick.

Once in Linux, a set of scripts allows you to:
- dump the original disk image on the usb stick.
- flash the device with a custom image, like a Debian.

## Download the image



## At first boot

### Get the network working

### Install klipper

### Flash the microcontroller and start Klipper


## Klipper config

rpanfili has already configured klipper for zortrax M200 but for an SK200 MCU board and he changed the step motors. However the geometry and a lot of stock parameters are already in his configuration.

Zortrax has published a Marlin port for the M200.
By looking at the commits, we can find the MCU pinouts to configure the klipper MCU firmware.
