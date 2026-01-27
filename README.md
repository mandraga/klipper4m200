# klipper4m200

A replacement firmare for the Zortrax M200+ 3D printer.

This firmware is based on Debian/Armbian and allows to install klipper on the m200+.
There is no comming back once the microcontroller is flashed.

## Access the internal single board comuter

You need:
* a serial port on the UART0 J5 connector for the u-boot and Linux serial console.
* a special USB-A to USB-A cable with no 5V connexion to be able to use the FEL mode.

At boot, press '2' on the serial console, it will start in FEL mode.
You should see this on "dmesg -w":
```
ID 1f3a:efe8 Allwinner Technology sunxi SoC OTG connector in FEL/flashing mode
```
Then, using sunxi-tools, you can upload a uboot image like this:
```
sunxi-tools$ sudo ./sunxi-fel uboot u-boot-sunxi-with-spl.bin
```
It starts uboot and you can browse files and dump data.

Then you can use this uboot prompt to load the dtb and the kernel, then boot Armbian from the usb stick.

Once in Linux, you can:
- dump the original disk image on the usb stick.
- dump the vendor u-boot env
- write your u-boot-env
- write u-boot-dtb.bin to the FAT16 partition

## Download the image

Compile u-boot with build_m200.sh from the u-boot folder in this repo, run it the u-boot fork repo.
Create an Armbian full image using the lime-A33 board using the armbian_build repo.
DD it on /dev/sda2 (or edit uboot command to use sda1 instead).
Do not forget to include linux headers and to enable the touchscreen (see linux.md).

## At first boot from FEL mode

Look at usb.md or progress.md to see how to load the kernel from u-boot.

Install the new u-boot env and mainline uboot binary on the emmc such as to automaticly boot from the USB drive.
The original emmc is not that much altered, only the env. You can resore it with a backup file or in vendor u-boot console.

### Get the network

On version 2.1, Linux should detect the ethernet usb adapter on the USB port.
On version 1.1, since USB0 was cut to break in, you need to connect some ethernet adapter to the usb port or hub before.
Connect your printer to the network.

### Install klipper

Follow this tutorial
https://travis90x.altervista.org/armbian-first-boot-and-install-klipper/

```
git clone https://github.com/dw-0/kiauh.git
./kiauh/kiauh.sh
```
It will install everything klipper related and build the firmware.

### Flash the microcontroller and start Klipper

See the Readme in the MCU folder, you need an ST-Link programmer to mass erase the chip.

## Klipper config

rpanfili has already configured klipper for zortrax M200 but for an SK200 MCU board.
However the geometry and a lot of stock parameters are already in his configuration.

Zortrax has published a Marlin port for the M200.
By looking at the commits, we can find the MCU pinouts in "pins_ZORTRAX_M200.h" to configure the klipper MCU firmware the other plus specific pinouts are in the zmxxxp-re repository.
Be carefull whith the Z axis stop, it is not the same.

The configuration in this repo is the rpanfili one, updated for the Zortrax MCU board instead of the SK1.4.

## Start printing

If everything went fine, you can connect to Mainsail or Fluid web interfaces copy the klipper configuration and start printing.

