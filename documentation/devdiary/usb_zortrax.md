
# Rev 2.2 and Rev 1.1

usb info

1: Hub,  USB Revision 2.0
 - u-boot EHCI Host Controller 
 - Class: Hub
 - PacketSize: 64  Configurations: 1
 - Vendor: 0x0000  Product 0x0000 Version 1.0
   Configuration: 1
   - Interfaces: 1 Self Powered 0mA
     Interface: 0
     - Alternate Setting 0, Endpoints: 1
     - Class Hub
     - Endpoint 1 In Interrupt MaxPacket 8 Interval 255ms

1: Hub,  USB Revision 1.10
 -  U-Boot Root Hub 
 - Class: Hub
 - PacketSize: 8  Configurations: 1
 - Vendor: 0x0000  Product 0x0000 Version 0.0
   Configuration: 1
   - Interfaces: 1 Self Powered 0mA
     Interface: 0
     - Alternate Setting 0, Endpoints: 1
     - Class Hub
     - Endpoint 1 In Interrupt MaxPacket 2 Interval 255ms

=> usb tree
USB device tree:
  1  Hub (480 Mb/s, 0mA)
     u-boot EHCI Host Controller 
   
  1  Hub (12 Mb/s, 0mA)
      U-Boot Root Hub 
   
### In Kernel log

[    0.565522] [usb_manager]: CONFIG_USB_SUNXI_USB0_OTG
[    0.565549] no usbc(0) det_vbus gpio and try to axp det_pin
[    0.565564] get usbc0(usb_restrict pin) failed

CONFIG_USB_SUNXI_USB0_OTG enables OTG and fixes the crash for Sunxi uboot branch in 2015

### Crash with USB0 enabled.
 
It was USB_GADGET and MUSB crashing the device in u-boot. Noo need for that.


### axp223

Le 5V entre à ACIN et sort à VBUS vers le connecteur USB0
La sortie vers VBUS (IPSOUT) est décidée soit par la pin N_VBUSEN à 0, soit par le bit 7 de 30H à 1 (default = 0x60, b0110 donc decidé par N_VBUSEN).

PH1 1=USB_slave 0=USB Host in usb_mode_script.sh -> connecté à usb ID pin 49 PH08 du SOM

USB_DRVVBUS, pin 25 du SOM va vers l'IC a 3 5 pattes ou est connecté IPSOUT (VBUS pin 48 du SOM).
USB_DRVVBUS est la pin 11 N_VBUSEN de l'axp223. C'est configuré en sortie sur le SOM.

Donc U4 est un transistor activé par N_VBUSEN.
* GND  IPDOUT selects VBUS
* High IPDOUT does not select VBUS

So DRIVEVBUS bit of 0x60 controls the output to the transistor+-.

#### FEX file

usb_id_gpio = port:PH08<0><1><default><default>
usb_det_vbus_gpio = "axp_ctrl"                     // to pin 49 PH08
usb_drv_vbus_gpio = port:power4<1><0><default><0>  // pin 25 to transistor?

### Enable USB0 VBUS using custom code

```
usb0vbus 1
```

### Clear PH1 in uboot to set 5V on VBUS

But linux will do it and we do not need in in u-boot.
```
gpio clear 225
```

### 2 USB systems

* USB OTG                NOK   Custom driver, musb suports it.
https://linux-sunxi.org/USB_OTG_Controller_Register_Guide

* USB 1   -> EHCI OHCI    OK

### Cables

The USB uses JST PH2.0 9mm connectors

https://fr.aliexpress.com/item/1005004723638618.html?spm=a2g0o.productlist.main.12.1a052492dYNovf&algo_pvid=ea23ed34-fa46-4497-9353-a5ebe9ef0fda&algo_exp_id=ea23ed34-fa46-4497-9353-a5ebe9ef0fda-11&pdp_ext_f=%7B%22order%22%3A%2218%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21EUR%211.19%211.21%21%21%211.36%211.38%21%402103856417616849636297202ea9d4%2112000030235832075%21sea%21FR%21177031303%21X%211%210%21n_tag%3A-29919%3Bd%3A9315576%3Bm03_new_user%3A-29895&curPageLogUid=tPvdFLsMdEuc&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005004723638618%7C_p_origin_prod%3A


