

## P3 Single Board Computer to Micro Controller Unit

It goes through a level shifter, to be investigated.

1  TX
2  RX
3  SLP
4  RST
5  GPIOS
6  LED
7  ADOHC
8  GPIO
9  GND
10 5V

## SDIO

The two first SD card slots are not populated. SDIO_0 goes to an unpopulated SDWifi chip.
SDIO_1 has no traces outside of the SOM board.
A test to use it with a card did not work.

## LED

Two onboard leds. Only the power led does something below the power button.
BTW the power button is more of a reset button.

## USB

Both usb are used.
USB1 on J4
USB0/OTG on P4

## Ethernet

