# Autopilot
_Autopilot_ is a **Quadcopter Autopilot** developed as part of the course "[Diseño de Sistemas Integrados](https://secretaria.uvigo.gal/docnet-nuevo/guia_docent/?centre=305&ensenyament=V05G301V01&assignatura=V05G301V01404&any_academic=2024_25)" in the Telecommunications Engineering Degree at the Universidad de Vigo (2024 - 2025).

## About The Project
This project implements an autopilot system for a quadcopter, designed to process sensor data, user commands, and control signals to maintain flight stability and responsiveness. The system is built using a modular microservices-based architecture, where each module communicates via the [Zenoh](https://zenoh.io) protocol, ensuring efficient and reliable data exchange. Key aspects include real-time data processing, PID control algorithms for flight stabilization, and a web interface for monitoring.

The project features:
- Sensor-based attitude estimation to determine drone orientation.
- PID control algorithms for stabilizing roll and pitch.
- Radio module integration for user command interpretation.
- Motor control system with real-time power adjustments.
- Zenoh-based communication for efficient data exchange between modules.
- Web interface for remote monitoring and control.

## How To Run
*This project was developed for a very specific system. If not using the same hardware and/or system architecture, heavy modifications will be required.*
### Hardware
This project was developed using the following hardware:
- [Emlid Navio2](https://docs.emlid.com/navio2/)
- [Raspberry Pi 3 Model B](https://www.raspberrypi.com/products/raspberry-pi-3-model-b/)
- [Futaba T6K](https://futabausa.com/product/6k-v3s/)
- [Futaba R3006SB](https://futabausa.com/product/r3006sb/)
- [Cineboy](https://es.aliexpress.com/item/1005001333855495.html)
- [Diatone MAMBA TOKA 1606 4300kv](https://www.diatone.us/collections/motor/products/mamba-toka-1606-motor-rotor-1-pcs-silver?variant=39307197513815)
- [Diatone MAMBA F40 MINI](https://www.diatone.us/collections/esc/products/mb-f40_128k-bl32-mini-esc)
- [U-TECH PRO 1550mAh 4S 14.8V 95C](https://rc-innovations.es/en/shop/blp00002356-u-tech-pro-1550mah-4s-14-8v-95c-lipo-battery-12846)

### Requirements
Follow [this guide](https://docs.emlid.com/navio2/configuring-raspberry-pi/) to download and install the preconfigured Raspberry Pi OS image.

Next, install Zenoh by following the steps outlined in [`Zenoh.pdf`](docs/Zenoh.pdf). The mentioned files are available in the [`dependencies/`](dependencies) directory.

### Compilation
Compile the project with:
```bash
mkdir src/modules/build
cmake -S src/modules -B src/modules/build
make -C src/modules/build

mkdir src/run/build
cmake -S src/run -B src/run/build
make -C src/run/build
```
These commands will generate the executable files in the `src/modules/build` and `src/run/build` directories.

### Calibration
Follow the steps in [`Calibrado.pdf`](docs/Calibrado.pdf) to calibrate the AHRS sensor.

If the [provided RTIMULib library](src/lib/RTIMULib/) is incompatible with your system, follow the instructions in [`RTIMULib.pdf`](docs/RTIMULib.pdf) to recompile it.

### Execution
#### System
Once compiled, you can run the system with:
```bash
sudo ./src/run/build/run
```

#### Constants
To modify the PID constants, run:
```bash
./src/modules/build/constants/constants
```

## About The Code
Refer to [`Presentación`](docs/Presentación.pdf) for a high-level overview of the project.

Refer to [`Specifications.pdf`](docs/Specifications.pdf), and [`Arquitectura.pdf`](docs/Arquitectura.pdf) for an in-depth explanation of the project, the system architecture, the different modules, the PID algorithm implementation, the hardware components, and more.