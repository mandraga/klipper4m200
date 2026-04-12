# klipper4m200

A replacement firmare for the Zortrax M200+ 3D printer.

This firmware is based on Debian/Armbian and allows to install klipper on the m200+.
There is no comming back once the microcontroller is flashed.

# Building steps

Update the submodules, or get them separately.
* Armbian build
* u-boot
* sunxi-tools

## Access the internal single board computer

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
sunxi-tools$ sudo ./sunxi-fel uboot ../u-boot/u-boot-sunxi-with-spl.bin
```
It starts uboot and you can browse files and dump data.

Then you can use this uboot prompt to load the dtb and the kernel, then boot Armbian from the usb stick.

Once in Linux, you can:
- dump the original disk image on the usb stick.
- dump the vendor u-boot env
- write your u-boot-env
- write u-boot-dtb.bin to the FAT16 partition

## Download the image

Compile u-boot with build_m200.sh from the bootstage folder in this repo, run it in the u-boot fork repo.
Create an Armbian full image using the lime-A33 board using the armbian_build repo.
DD it on /dev/sdx2 where sdx is your usb stick (or edit uboot command to use sdx1 instead).
Do not forget to include linux headers and to enable the touchscreen (see linux.md).
In armbian_build:
```
./compile.sh INSTALL_HEADERS=yes
```

## At first boot from FEL mode

Look at usb.md or progress.md to see how to load the kernel from u-boot.

Install the new u-boot env and mainline uboot binary on the emmc such as to automaticly boot from the USB drive.
The original emmc is not that much altered, only the env. You can resore it with a backup file or in vendor u-boot console.

### Get the network

* On version 2.1, Linux should detect the ethernet usb adapter on the USB port.
  Version 2.1 has two usb ports.
* On version 1.1, the USB0 trace must be cut and a posrt soldered to boot from USB. You need to connect some ethernet 
adapter to an usb hub.
  Version 1.1 has only one USB port on the front.

Then connect your printer to the network.

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
By looking at the commits, and the work done in the zmxxxp-re repository, I could find the MCU pinouts in "pins_ZORTRAX_M200.h" to configure the klipper MCU firmware.
Be carefull whith the X, Y axis stops, they are not the same depending on verison 1.1 or 2.2.

The configuration in this repo is the rpanfili one, modified for the Zortrax MCU board instead of the SK1.4.

## Calibration and tuning

https://ellis3dp.com/Print-Tuning-Guide/
https://github.com/OrcaSlicer/OrcaSlicer/wiki/Calibration

The ZABS spec
https://cf.zortrax.com/wp-content/uploads/2020/09/Z-ABS_2_Technical_Data_Sheet_eng.pdf

PID tuning at different fan speeds and in GCODE:
https://www.youtube.com/watch?v=USvg6D0cXE4

## Start printing

If everything went fine, you can connect to Mainsail or Fluid web interfaces copy the klipper configuration and start configuring the bed level.
Then, because it is klipper, you must run calibration towers with your filaments and tweak orcaslicer prior to begin printing. That's what's lost in the process but you have control and can use any filament.


## About

This work lasted from october 2025 to April 2026 and was much more difficult than planed, given that the device tree of this thing is really complex, the EMMC is really touchy, and the screen had no driver.
The boot process investigation led to 4 bricks, and needed resoldering a new programmed EMMC to save the boards.
Since then, I tryed to port an ezeetab 96Q10 A-33 Andorid tablet to linux and it was quicker (a week) and also helped to progress on this project.

