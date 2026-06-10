# RoboMaster Biped Serial-leg Robot

## Project Overview
A quadruped robot project based on RoboMaster Type-C development board with Damiaotech motors.
Complete theoretical verification is in the PDF.

## Hardware Configuration
- **Development Board**: RoboMaster Type-C Development Board
- **Joint Motor**: Damiaotech 6215 (for joint actuation)
- **Drive Motor**: Damiaotech 4310 (for wheel drive)

## Project Structure
```
robomaster-c/
├── hardware/           # Hardware design files
├── software/           # Software code
│   ├── src/           # Source code
│   ├── include/       # Header files
│   └── lib/           # Libraries
├── config/            # Configuration files
├── docs/              # Documentation
└── README_en.md       # This file
```

## Hardware Connections

### Damiaotech 6215 (Joint Motor)
- For precise joint control
- Connected to RoboMaster Type-C development board CAN interface

### Damiaotech 4310 (Drive Motor)
- For wheel propulsion
- Connected to RoboMaster Type-C development board PWM/CAN interface

## Software Architecture

### Core Modules
- `motor_ctrl/`: Motor control module
- `leg_controller/`: Leg motion control
- `motion_planner/`: Motion planning
- `communication/`: Communication interface

## Build and Flash

### Requirements
- CMake 3.10 or higher
- ARM GCC cross-compilation toolchain
- STM32F405 libraries

### Build Steps
```bash
mkdir build
cd build
cmake ..
make
```

## Feature Checklist

- [ ] Motor drive initialization
- [ ] CAN bus communication
- [ ] Joint motor control
- [ ] Drive motor control
- [ ] Motion control algorithms
- [ ] State feedback
- [ ] Remote control interface

## References
- RoboMaster Type-C Development Board Documentation
- Damiaotech Motor Communication Protocol
- MATLAB Simulation Model (see ../matlab folder)

## Author
Created: 2026-05-09
