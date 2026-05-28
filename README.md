# What Is This

DONT GET CONFUSED this is not some missile controller LOL this is for MODEL ROCKETS ONLY! So they dont interfere with the flight paths of flights and light weight
Aircrafts While launching your Model Rockets

# HoW TO USE

# Rocket Guidance Program Documentation

## Overview
This program guides a rocket to a target GPS location by reading from an onboard GPS module and calculating the optimal trajectory.

## Hardware Requirements
- Any GPS module (UBLOX, SIRF, or MTK compatible)
- UART-compatible microcontroller (Arduino/Raspberry Pi)
- Power supply and appropriate voltage levels

## Installation
1. Connect GPS module to UART pins:
   - VCC → 3.3V (or 5V if supported)
   - GND → Ground
   - TXD → RX pin (with level shifter if needed)
   - RXD → TX pin (with level shifter if needed)

2. Compile the program:
   ```bash
   gcc -o rocket_guidance rocket_guidance.c -lm
   ```

## Usage
1. Run the program:
   ```
   ./rocket_guidance [/dev/ttyUSB0]
   ```

2. When prompted, enter target coordinates:
   ```
   Enter target coordinates (lat lon):
   ```

3. The program will display:
   - Detected GPS model
   - Current position
   - Distance to target
   - Bearing angle

## Customization Options
1. **Device Path**: Specify custom UART device path:
   ```
   ./rocket_guidance /dev/ttyS0
   ```

2. **Timeout**: Adjust GPS timeout in seconds by modifying `GPS_TIMEOUT` constant.

3. **Accuracy**: Add altitude correction by modifying `calculateTrajectory()` function.

4. **Motor Control**: Add motor control commands at the end of `main()`:
   ```c
   // Example pseudo-code for motor control
   setMotorDirection(bearing);
   setMotorSpeed(distance);
   ```

## Troubleshooting
- If GPS isn't detected: Check wiring and device permissions.
- If coordinates are inaccurate: Ensure GPS has clear sky view.
- For high-precision applications: Add filtering algorithms.

## Example Output
```
Detected GPS model: UBLOX
Current position: 40.7128,-74.0060
Enter target coordinates (lat lon): 34.0522,-118.2437

Trajectory calculated:
Distance: 3935278.2m
Bearing: 315.23°
```

This program provides precise navigation guidance for rockets with minimal hardware requirements.
