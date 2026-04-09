### Prime line to begin extrusion before the printed object

```
; Prime line BEGIN
G1 X2.0 Y2.0 F18000 ; Move to edge of bed
G1 Z0.2 F5000 ; Lower nozzle
G92 E0.0 ; Reset extruder position
G1 X200.0 E32.0 F1000.0 ; Start intro line
G1 X2.0 Y2.24 F18000 ; Move out of the first line
G1 X50.0 E14.0 F1000.0 ; Finish thicker line
G92 E0.0 ; Reset extruder position again
; Prime line END
```


### Start code

Just homing, then add the prime line after the temperature is stable.

```
G28
```

### End code

Home and stop the heaters.
Boost the fans for a couple seconds?
Emit a sound using the Z axis ?

```
PRINT_END
G28
M104 S0
M140 S0
```
