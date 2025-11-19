## MCU compilation for the MCU board

The MCU is an STM32F103VE on the M200 plus and STM32F103VC on the M200 and the stepper motor drivers are A4988.

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

### Build the firmware

```
./kiauh/kiauh.sh
Select
 4) [Advanced] 
  1) [Build] 
```
And then select the STM32F103VE with bootloader at 28K, and UART communication.

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
-	TMS
-	TCK
-	**TX <--- connect to RX on host**
-	**RX <--- connect to TX on host**
-	NC
-	VCC (marked on mainboard as 3.3V)

**Caution**: if you are going to use debugger (ST-Link), know that any attempt to read or write from/to flash memory will result in mass erase of the flash. It will erase the the bootloader and all the settings, including lifetimer, serial number and hardware version, as flash memory of chip is read out protection enabled at production! If that happens you won't be able to use official firmware anymore!


### Programming the firmware on the machine:


