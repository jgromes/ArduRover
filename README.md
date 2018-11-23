# ArduRover

## Open-source, full metal 6WD robot

---

Repository contents:
* **android** - All Android control app source files.
* **apk** - Android application package file. Install on your own risk!
* **arduino/arduroverController** - USB control sketch, runs on Arduino, takes in Serial input from PC and transmits the commands to the robot via LoRenz shield.
* **arduino/arduroverControllerBluetooth** - Bluetooth control sketch, same as the above but works with bluetooth and the Android control app.
* **arduino/arduroverMain** - The on-board sketch that controls the robot, receives commands via LoRenz shield and executes them.
* **doc** - Part design files (PDF drawings and Solidworks part files).

---

Find out more about the project here:
* Part 1 - <http://www.deviceplus.com/how-tos/arduino-guide/arduino-explorer-rover-chassis-final-assembly/>
* Part 2 - <http://www.deviceplus.com/how-tos/arduino-guide/arduino-explorer-rover-part-2-electronics-wiring/>
* Part 3 - <http://www.deviceplus.com/how-tos/arduino-guide/ardurover-arduino-explorer-rover-part-3-programming/>
