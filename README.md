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

Below are instructions to setup development of this project. These are intended for anyone to follow. If you are computer savy, you can probably ignore some of the steps if you want to do it a different way.
* Do the following steps to install the WSL at [this link](https://docs.microsoft.com/en-us/windows/wsl/install-win10).
  * Do step 1 then restart your computer.
  * Do step 6 and pick Ubuntu 18.04 LTS for the distribution.
* Run the following command to install development tools you will need:
```console
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
```
* Follow these steps combined referencing [these instructions](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/) for setting up an ESP32 development environment.
  * Run the following command from step 2
```console
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
```
  * Run the following command from step 3
```console
cd ~/esp/esp-idf
./install.sh
```
  * Run the following command from step 4
```console
. $HOME/esp/esp-idf/export.sh
```
* In the `esp` directory, clone this repository:
```console
ok@computer:~/esp$ git clone https://github.com/NU-Motorsports/dyno.git
```
* Connect the board to your computer. Find number for the COM port that the device connected to.
* Replacing the number for the COM port number, run the following command: (Replace <N> with your port number)
```console
sudo chmod 666 /dev/ttyS<N>
```
* Run the following command to enter the project director.
```console
cd ~/esp/dyno
```
* Run the following command to build, flash, and monitor the code. (Replace <N> with your port number)
```console
idf.py -p /dev/ttyS<N> -b 115200 flash monitor
```
