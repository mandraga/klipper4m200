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

Install the u-boot data on the emmc such as to automaticly boot from the USB drive.
The original emmc is not that much altered, only the sunxi u-boot. You can resore it
from the console or with a script writing the original u-boot/m200plus_sunxiuboot_env.img.backup file.

### Get the network working

On version 2.1, Linux should detect the ethernet usb adapter on the USB port.
On version 1.1, since USB0 was cut to break in, you need to connect some ethernet adapter to the usb port or hub before.
Connect your printer to the network.

### Install klipper

Follow this tutorial
https://travis90x.altervista.org/armbian-first-boot-and-install-klipper/

### Flash the microcontroller and start Klipper

Select STM32103CE, uart communication and 28Kb bootloader offset.

## Klipper config

rpanfili has already configured klipper for zortrax M200 but for an SK200 MCU board.
However the geometry and a lot of stock parameters are already in his configuration.

Zortrax has published a Marlin port for the M200.
By looking at the commits, we can find the MCU pinouts in "pins_ZORTRAX_M200.h" to configure the klipper MCU firmware.

The configuration in this repo is the rpanfili one, updated for the Zortrax MCU board instead of the SK1.4.

## Start printing

If everything went fine, you can connect to Mainsail or Fluid web interfaces copy the klipper configuration and start printing.

