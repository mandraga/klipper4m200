# Adapting a Dragon burner head to the zortrax m200

## Why?

* Your heater element is broken or missing
* You want more up to date tech
* You bough a Zortrax M200 or M300 without heating element.

## How?

* Print an adapter
  Use the dragon burner configurator https://dragonburner.voron.zip/DragonBurnerConfig/#/
* Chose your heat element
* Solder the fan cables to extend them (maybe you can reuse the heater element fan)
* Change the thermistor for a themocouple (like 1188-TC-K-TYPE-ND form Olimex)

## What heat element?

I chose a Rapido and a TZ-V6-2. But you have to solder things and it is anoying.
The best choice would be a Phaetus Dragon hotend because you do not change the heating elements
nor the thermocouple. And with a bit of luck with the fans cable length, you do not have to solder.

### Phaetus Rapido

And download the main body and the rapido adapter.
Instead of an extruder mount, print dragonburnermountonM200_B.stl
You need 35mm 5mm/6mm M3 screws (flat head for the 6mm long screw).
Replace the thermistor with a thermocouple.

### Triangle labs TZ-V6-2

This one is not really suported by the dragon burner project.
You need 35mm 5mm/6mm M3 screws (flat head for the 6mm long screw).
Replace the thermistor with a thermocouple.
Print the main fan body and this adapter:
https://www.printables.com/model/1292661-dragonburner-tz-v6-20-mount-with-v6-nozzle
Add washers on the support because the heater wobbles.
Change the nozzle to a V6 because the default nozzle is not made for that mount.
It will be at blower level and won't work.

### Dragon hotend

Not tested, but should be the simplest. Using the original heater and thermocouple even if the wire are a bit short. It is the first choice for the Dragon burner and it is afordable.
You need 35mm or 5mm/6mm M3 screws (flat head for the 6mm long screw).
