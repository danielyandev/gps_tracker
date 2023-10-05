# GPS Tracker
GPS tracker using Arduino nano, SIM868 GSM module, and NEO-6M GPS module


# Components used

### Arduino nano
<img src="https://i.ibb.co/X3fZTWF/NANO3-USBC-002.jpg" width="350">

### SIM868 GSM module
<img src="http://www.chinalctech.com/uploads/allimg/191217/1-19121GP250147.jpg" width="350">

### NEO-6M GPS module
<img src="https://miro.medium.com/v2/resize:fit:558/1*60Lz1-PTkQaEbA7a02pOLw.png" width="350">

# Wiring together

| Arduino pin | Board | pin |
| ----------- | ----- | --- |
| d2 | SIM868 | tx |
| d3 | SIM868 | rx |
| gnd | SIM868 | gnd |
| d8 | NEO-6M | tx |
| d9 | NEO-6M | rx |
| 5v | NEO-6M | vcc |
| gnd | NEO-6M | gnd |

# Powering

7.4v (2s) li-ion pack was used for power.
GPS module uses arduino "5v" pin, but GSM module and arduino should be powered by battery pack.
Connect "-" to shared gnd, "+" to arduino "vin" pin and GSM module vcc.

# Arduino LED signals
- 3 1s blinks: initialized (one-time action, only on start)
- 3 300ms blinks: sending data to the server (typically every 5 sec)

# SIM868 LED signals
- 800ms blinks: search for network
- 1s blinks: connected to network
- 300ms blinks: ready for GPRS

# NEO-6M LED signals
- no blinks, solid red light: search for satellites
- blinking: satellites found
