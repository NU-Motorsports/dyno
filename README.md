# NU Baja DAQ System

This program and set of libraries is intended for use as a dynomometer control and data aquisition system for the Northeastern Baja Racing team.

This project uses an ESP32 and the ESP-IDF API.

## Functionality Overview

### Input

Collects the following inputs and stores them in CSV format on a microSD card:

Value | Sensor
--- | ---
CVT Primary RPM | Hall Effect
CVT Secondary RPM | Hall Effect
Engine Output Shaft Torque | Torque Transducer
CVT Belt Temperature | IR Sensor
3 Other Temperatures (TBD) | Thermistor
Hysteresis Brake Torque | Load Cell
Throttle Position | Potentiometer

### Output

Controls the following variables using PWM

Variable | Via
--- | ---
Hysteresis Brake Torque | Current
Throttle Position | Servo

## Development Setup

Below are instructions to setup development of this project.

* Follow [these instructions](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/) for setting up an ESP32 development environment.
* In the `esp-idf` directory, clone this repository.
```console
ok@computer:~/esp/esp-idf$ git clone https://github.com/harrisonmg/dyno.git
```
* In the cloned repository, use `make menuconfig` to set the proper port of the connected ESP32.
* Use `make flash monitor` to build the project, flash it to a connected ESP32, and begin the serial monitor.
