# Makefile to compile main.h (assuming main.c exists) for ARM architecture

CC=arm-linux-gnueabihf-gcc
CFLAGS= -Wall
LDFLAGS=
TARGET=vendor_protocol_programing_tool
SRC=main.c

INCLUDE_DIR=/home/patrick/moredata/emmc/armbian_build/cache/sources/linux-kernel-worktree/6.1__sunxi__armhf/usr/include

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

# scp -o IdentitiesOnly=yes vendor_protocol_programing_tool root@192.168.1.158:/home/patrick/