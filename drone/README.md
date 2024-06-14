# drone

## Description

This project is part of my larger Quadcopter Drone project. The code in this
directory is a FreeRTOS app that serves as the flight controller and
transceiver for a drone controlled by a Bluetooth remote control. The code for
the remote control is located in the other directory.

## Setup

If you wish to run the program, below are the steps:

### Software setup

I used the ESP IDF for development of this project. In order to recreate
this example project, you will have to install the ESP IDF as explained at
[https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html)

Once you've set it up (as well as made the necessary hardware connections) you need to...

#### 1. Install the Drone Project

```bash
git clone git@github.com:JSpeedie/Quadcopter-Drone.git Quadcopter-DroneGit
cd ~/esp/esp-idf
mkdir projects
cp -r ~/Quadcopter-Drone/drone/ projects/drone
```

Make sure you check and enable `Bluetooth` under `Component config -->
Bluetooth`:

```bash
idf.py menuconfig
```

#### 2. Compiling and Running

After installing the project (and making the hardware connections. See below),
we are finally ready to compile and run the project:

```bash
cd ~/esp/esp-idf
get_idf
cd projects/drone
idf.py -p /dev/ttyUSB0 flash monitor
```

### Hardware connections

Below is the schematic I used for wiring up the drone.

<!-- <p align="center"> -->
<!--   <img src="https://raw.githubusercontent.com/wiki/JSpeedie/embedded-scribbles/images/ESP32-Tilting-Ball.png" width="50%"/> -->
<!-- </p> -->

Of course you will also need to connect a micro usb to usb cable between the
ESP32 and your development machine in order to flash the program to the ESP32.
