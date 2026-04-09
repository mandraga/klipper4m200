

## P3 Single Board Computer to Micro Controller Unit

It goes through a level shifter, and the labels are obfuscated.

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

The first SD card slots is not populated. 
SDIO_0 has no traces outside of the SOM board, SDIO_1 goes to an unpopulated SDWifi chip..
A test to use it with a card did not work.

## LED

Two onboard leds. Only the power led does something below the power button.
BTW the power button is more of a reset button.

## USB

Both usb are used.
USB1 on J4
USB0/OTG on P4

## Ethernet

On rev1 we have a soldered realtek ehternet chip. We cut it out in order to solder an usb hub.
On rev2 we have an USB2.0 Hub and the realtek ehternet chip is there. No need to cut or solder anything.
