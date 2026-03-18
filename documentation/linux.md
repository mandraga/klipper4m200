
## Menuconfig the kernel compilation.

In the armbian image repository (https://github.com/armbian/build.git)

./compile.sh INSTALL_HEADERS=yes
and chose to show a menuconfig

Configure to use the lime A33 system.

## Touch screen information

https://forum.armbian.com/topic/7233-bananapi-pro-7-touchlcd-with-debian-buster-and-mainline-kernel-5xxxx/

## LCD information

The screen is a "MTF0397SWI-06"
IC: OTM8019A
Sunxi driver: sl698ph_720p
We also find the name hx8394d in the driver functions.
Himax HX8394 is the mainline driver name for hx8394d, looks like the sunxi driver, same command codes but different data sizes in the two drivers.

In the sl698ph_720p driver it takes hx8394d or otm1283 depending on the identification result.

On a boot parition, we find lcd.ko containing sl698ph_720p but also lp079x01.
lp079x01 is an LG display.
The driver has been compiled with debug information. "otm1283_initialization_setting" apears in the debug info.
So we have the driver source code, we need to adapt it to mainline linux.

We find lcd names: tft720x1280 vvx10f004b00 lp907qx starry768x1024

Acording to the debug info, the lcd module includes sl698ph_720p.c. However his initialisation data is not the same as in the public sunxi driver, it must be extracted from the module on the printer.

# Possible modern drivers to be used as "donor" structure

https://github.com/l33tnoob/tinno82_S5501/blob/master/mediatek/custom/common/kernel/lcm/otm1283a_hd720_dsi_vdo/otm1283a_hd720_dsi_vdo.c

The term "LCM" (Liquid Crystal Module drivers) is commonly used in MediaTek and some Allwinner/Android vendor kernels. You’ll often see files named like otm1283a_hd720_dsi_vdo.c or hx8394d_dsi_vdo.c in directories such as kernel/lcm/ or kernel/drivers/lcd/. LCM drivers are a vendor-specific concept and are not part of mainline Linux. Mainline Linux uses the DRM/KMS subsystem and expects panel drivers to implement the struct drm_panel interface.

We take the closest mainline driver from the Armbian sources.

# Writing the driver

We take the panel-himax-hx8394.c file structure and use the pannel init from "otm1283_initialization_setting" in sl698ph_720p.c with the data extracted from the lcd kernel module on the printer.

### LCD pannel driver compilation

To compile panel-mtf0397swi.c as a kernel driver for Armbian, you need to integrate it into the kernel source tree and build it as part of the kernel or as a loadable module. 

https://www.kernel.org/doc/html/latest/kbuild/modules.html

Compilation

```
scp -o IdentitiesOnly=yes Makefile root@192.168.1.158:/home/patrick/lcd_driver
scp -o IdentitiesOnly=yes panel-mtf0397swi.c root@192.168.1.158:/home/patrick/lcd_driver
echo "obj-m += panel-mtf0397swi.o" > Makefile
make -C /lib/modules/`uname -r`/build M=$PWD
```

Installation

```
sudo cp panel-mtf0397swi.ko /lib/modules/$(uname -r)/kernel/drivers/gpu/drm/panel/
sudo depmod -a
sudo modprobe panel-mtf0397swi
```

### Testing the screen

```
dmesg | grep drm
dmesg | grep dsi
```
bootdelay
bootdelay
```
# This reload my be needed on the nearly working version.
sudo modprobe -r panel-mtf0397swi
sudo modprobe panel-mtf0397swi

modetest -c

apt update && sudo apt install libdrm-tests mesa-utils kmscube
kmscube

# Display a 800x600 image (stop klipperscreen if installed: "sudo systemctl stop KlipperScreen")
sudo apt install fbi
fbi -T 1 -d /dev/fb0 -a ../OIP-2008394785.jpeg 
```

 ### Rotate the screen for KlipperScreen

```
sudo nano /etc/X11/xorg.conf.d/01-armbian-defaults.conf 
```
Put this in the file:
```
Section "Monitor"
    Identifier "DSI-1"
    Option "Rotate" "left"
EndSection
```

Restart KlipperScreeen
```
sudo systemctl restart KlipperScreen
```

## Compilation

Go to the armbian build repo and call "./compile.sh INSTALL_HEADERS=yes", then select kernel compilation.
TODO: add the zortrax to armbian "at your own risk" boards
Select current kernel with:

- touch screen in 
Device Drivers --->
    Input device support --->
        Touchscreens --->
            EDT FocalTech FT5x06 I2C Touchscreen support"

CONFIG_TOUCHSCREEN_EDT_FT5X06

You will need to compile the lcd driver for your armbian, using the armbian_build git repository.

### Compilation on the target

Assuming the image has the headers.
./compile.sh INSTALL_HEADERS=yes

### Cross compilation with the armbian build kernel source

make
and then copy on the system

Copy in
/lib/modules/6.12.51-current-sunxi/kernel/drivers/

Does not work because of some timestamp anf debug info not matching the image

### Inserting the module code in the source

sudo cp panel-mtf0397swi.c /home/patrick/moredata/emmc/armbian_build/cache/sources/linux-kernel-worktree/6.12__sunxi__armhf/drivers/gpu/drm/panel

sudo nano Kconfig

config DRM_PANEL_MTF0397SWI
        tristate "Microtech Technology MTF0397SWI-06 panel"
        depends on OF
        depends on DRM_MIPI_DSI
        depends on BACKLIGHT_CLASS_DEVICE
        help
          Say Y here if you want to enable support for Microtech Technology MTF0397SWI panel.

sudo nano Makefile

obj-$(CONFIG_DRM_PANEL_ABT_Y030XX067A) += panel-mtf0397swi.c.o

Then run

export CROSS_COMPILE=arm-linux-gnueabihf-
make ARCH=arm zImage -j$(nproc)
make ARCH=arm modules -j$(nproc)
This does not work, it must be armbian specific stuff

Trying with the config thing

### With compile.sh

https://docs.armbian.com/Developer-Guide_Build-Switches/

./compile.sh INSTALL_HEADERS=yes

Then on the device the headers are in

/usr/src/linux-headers-6.12.51-current-sunxi/


https://forum.armbian.com/topic/6-how-to-build-my-own-image-or-kernel/

### Compiling on the system

scp panel-mtf0397swi.c  

https://www.kernel.org/doc/html/latest/kbuild/modules.html
```
echo "obj-m += panel-mtf0397swi.o" > Makefile
```

From the driver soure directory:

```
make -C /lib/modules/$(uname -r)/build M=$PWD
```

Using bleeding edge 6.16 kernel is a bad idea, cmpilation does not work, and many errors in dmesg.

./compile.sh kernel BOARD=lime-a33 BRANCH=current RELEASE=trixie BUILD_MINIMAL=yes SOURCE_COMPILE="yes" 

### Testing the touchscreen

```
sudo apt update
sudo apt install -y evtest evemu-tools libinput-tools i2c-tools
evtest
```

### Check the device tree

```
dtc -I fs /proc/device-tree | sed -n '/panel@/,/};/p' 
```


### Camera

It is a GalaxyCore GC2145 2MP sensor
Already enabled CONFIG_VIDEO_GC2145=m
But the csi driver seems not to be compatible and the i2cdetect gives nothing.
Maybe an enable pin problem.
So for now it stays deactivated.

Drivers on the vendor system:

./vendor/modules/gc0307.ko
./vendor/modules/gc0308.ko
./vendor/modules/gc0309.ko
./vendor/modules/gc0311.ko
./vendor/modules/gc0328.ko
./vendor/modules/gc0329.ko
./vendor/modules/gc2035.ko
./vendor/modules/gc2155.ko
./vendor/modules/gc5004.ko
./vendor/modules/gc5004_mipi.ko

./vendor/modules/ov12830.ko
./vendor/modules/ov13850.ko
./vendor/modules/ov16825.ko
./vendor/modules/ov2640.ko
./vendor/modules/ov5640.ko
./vendor/modules/ov5647.ko
./vendor/modules/ov5647_mipi.ko
./vendor/modules/ov5648.ko
./vendor/modules/ov5650.ko
./vendor/modules/ov7736.ko
./vendor/modules/ov8825.ko
./vendor/modules/ov8825_act.ko
./vendor/modules/ov8850.ko
./vendor/modules/ov8858.ko
./vendor/modules/ov8858_4lane.ko

#### Installation

We place the module in the image, and add compatible "MTF0397SWI-06, sl698ph_720p" to the device tree pannel object.
