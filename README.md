# Quadcopter-Drone

A four motor remote control drone, built from scratch with an ESP32 and featuring a custom made Bluetooth controller.

## Project Layout

There are 2 sub-projects to this project: 

1. The FreeRTOS app that runs on the drone itself (located in the `drone` directory)
2. The FreeRTOS app that runs on the remote control (located in the `remote-control` directory)

In order to build this project, you must wire up both the drone and the
remote control according to the wiring diagrams found in the READMEs of both
directories (in progress atm), and then flash the code from each directory
to their respective ESP32s.

Please visit the `drone` and `remote-control` directories for more.
