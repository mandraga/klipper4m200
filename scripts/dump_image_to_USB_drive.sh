#!/bin/bash

# Reads a big partition in multiple files

# configure serial port and open it for read/write
stty -F /dev/ttyUSB1 115200 -parenb raw

# Emmc size = 15368192 sectors
#0x8000 = 32768
# 469 fichiers
MMC_SECTOR=0
SIZE=32768

# send a CR to wake the prompt
printf '\r' > /dev/ttyUSB1
printf 'mmc dev 1\r' > /dev/ttyUSB1
sleep 0,5
printf 'start usb\r' > /dev/ttyUSB1
sleep 0,5
for i in {0..469}; do
    MMC_ADDRESS=$(printf "0x%x" "$MMC_SECTOR")
    echo "Reading MMC at $MMC_ADDRESS"
    READ_CMD="mmc read 0x42000000 $MMC_ADDRESS 0x8000"
    WRITE_CMD="fatwrite usb 0:1 0x42000000 emmc.img$i 0x1000000"
    # send commands with explicit CR, do not rely on echo escaping
    printf '%s\r' "$READ_CMD" > /dev/ttyUSB1
    sleep 1
    RESULT=$(timeout 2 cat /dev/ttyUSB1 2>/dev/null || true)
    echo "read: $RESULT"
    sleep 1
    printf '%s\r' "$WRITE_CMD" > /dev/ttyUSB1
    sleep 2
    # advance address and keep hex format
    MMC_SECTOR=$((MMC_SECTOR + SIZE))
    # read from the serial device with a timeout so we don't block forever
    RESULT=$(timeout 2 cat /dev/ttyUSB1 2>/dev/null || true)
    echo "$i write - $RESULT"
done
