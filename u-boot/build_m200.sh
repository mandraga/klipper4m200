#!/bin/sh
#sudo apt install build-essential
#sudo apt install arm-linux-gnueabihf-gcc
#sudo apt install gcc-arm-linux-gnueabihf
#sudo apt install libssl-dev 
#sudo apt install libgnutls28-dev 

make CROSS_COMPILE=arm-linux-gnueabihf- A33-ZortraxM200Plus_defconfig
make CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
cp u-boot-sunxi-with-spl.bin ../sunxi-tools/
