## MCU compilation for the MCU board

The MCU would be a STM32F103VC on the M200 and the stepper motor drivers are A4988.
Format LQFP100

STM32F103VC
Flash memory: 256 KB
SRAM: 48 KB
Package: LQFP100
I/O pins: Up to 80

STM32F103VE
Flash memory: 512 KB
SRAM: 64 KB
Package: LQFP100
I/O pins: Up to 80

### From the Marlin port

```
#
# Zortrax M200/M300 (STM32F103VCT6)
#
[env:STM32F103VC_zortrax_m200]
extends                     = stm32_variant
board                       = genericSTM32F103VC
board_build.variant         = MARLIN_F103Vx
build_flags                 = ${stm32_variant.build_flags} -DDEBUG_LEVEL=0
monitor_speed               = 115200
board_build.offset          = 0x8000       (32768 bytes, 32K offset)
board_upload.offset_address = 0x08008000
```

### Build the firmware

```
./kiauh/kiauh.sh
Select
 4) [Advanced] 
  1) [Build] 
```

And then select (Enable extra low-level configuration options):
Micro-controller Architecture: STM32
Processor model:               STM32F103
Bootloader at:                 8K for katapult, 32K if it was the zortrax bootloader.
Communication interface:       USART2 on PA3 PA2
Frequency                      8Mhz

It will build the firmware.

### Configure

The firmware is generic, it needs a configuration file to assign the pins and other stuff.
The configuration can be deduced from the Zortrax Marlin port. See in "pins_ZORTRAX_M200.h".

The other part of the work for the printer configuration has been made by rpanfili in his sk200 retrofit.
He used an sk200 and he changed the stepper motors and mybe the extruder but the other parts are the same.

#### Serial communication

To connect serial user interface (OctoPrint or other using Raspberry PI or UART converter and PC) you can use debug header on the motherboard. You will need to solder connector by yourself.

Pinout of the **DEBUG** header described below:
-	NRST
-	**GND <--- connect to GND on host**
-	TMS/NC
-	TCK/NC
-	**TX <--- connect to RX on host**
-	**RX <--- connect to TX on host**
-	BOOT0
-	VCC (marked on mainboard as 3.3V)

**Caution**: if you are going to use debugger (ST-Link), know that any attempt to read or write from/to flash memory will result in mass erase of the flash. It will erase the the bootloader and all the settings, including lifetimer, serial number and hardware version, as flash memory of chip is read out protection enabled at production! If that happens you won't be able to use official firmware anymore!

### Pins

Android side       MCU side

5V                 5V
GND                GND
GPIO               STM
ADOHC              MTS
LED                BOOT
GPIOS              GPIO
RST                RST
SLP
RX                 RX
TX                 TX

MCU   Android pins
-------------------------------------------------------------------------------------------------
STM   is PD2 TIM3_ETR/UART5_RX/SDIO_CMD     gpio_pin_5 = port:PC01<1><default><default><0>  MISO
MTS   is PD1 OSC_OUT/FSMC_D3/CAN_TX         gpio_pin_6 = port:PC00<1><default><default><0>  MOSI
BOOT  is PD3 USART2_CTS                     gpio_pin_7 = port:PC02<1><default><default><1>  CLK
GPIO  is PD5 USART2_TX                      gpio_pin_8 = port:PC03<0><1><default><0>        CS
RESET is PD4 USART2_RTS                     gpio_pin_9 = port:PF01<1><default><default><0>  
SLP   is not connected                      gpio_pin_10 = port:PF00<1><default><default><0>
RX    is PA3 USART2_RX                      PB0
TX    is PA2 USART2_TX                      PB1

uart2_cts_rts_pb_pins: uart2-cts-rts-pb-pins {
    pins = "PB2", "PB3";
    function = "uart2";
};

Debug connector:

RESET; VSS/GND; PA13;     PA14;        PA10;       PA9;      BOOT0; 3.3V
              JTMS-SWDIO  JTCK-SWCLK  USART1_RX  USART1_TX

1 3.3V (square pin)
2 BOOT0
3 PA9      USART1_TX
4 PA10     USART1_RX
5 PA14     JTCK-SWCLK
6 PA13     JTMS-SWDIO
7 VSS/GND
8 RESET

### Katapult configuration

If you replace the vendor bootloader with a katapult one using the debug port.

make menuconfig
```
Micro-controller architecture: STM32F13
No deployment application
Clock reference: 8Mhz crystal
Communicaiton iunterface: USART2 PA3/PA2
Application start offset 8KB offset.
//Enable bootloader on button state: PD4 RESET
```

make

It generates 
```
out/katapult.bin
```
That's what we flash as bootloader.

Using the Katapult's flashtool on the debug header.
```
sudo python3 flashtool.py -d /dev/ttyUSB0 -b 115200 -f out/katapult.bin
```
or

```
sudo stm32flash -b 115200 -w out/katapult.bin /dev/ttyUSB0
```

### Some issue with keeping the bootloader active

https://github.com/Arksine/katapult/issues/135

### Programming the firmware on the machine:

```
sudo apt update
sudo apt install python3-serial
python3 flashtool.py -d <serial device> -b <baud_rate> -f ~/klipper/out/klipper.bin
```
