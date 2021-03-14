
# Firmware

The firmware is based on the zephyr rtos. The integrated shell over uart is used to control the machine.

## Requirements
A working zephyr environment version >= v2.5 is required to build the firmware. ([Zephyr getting started](https://docs.zephyrproject.org/latest/getting_started/index.html))

## Supported boards

The firmware should run on every board supported by zephyr that has gpio and uart support and is clocked fast enough. Tested on a nucleo_f303re board. 

## Configuration

To configure what pins control the stepper motors edit the stepper_config.overlay file.

## Build and flash

```
west build --board nucleo_f303re
west flash
```

## Usage

To use the machine connect to the interactive shell over the serial connection (use screen or putty for example).

For commands that require a stepper id the following mapping applies:
- Stepper id 1: Cutter
- Stepper id 2: Mover

### Disable steppers

To disable all steppers, for example to load a new tape or position the knife manually, exeute:

```
hacker disable_steppers
```

### Knife
To manually lower or rise the knife execute
```
hacker knife knife_pos
```
knife_pos can either be "up" or "down"

### Move
There are two move commands
- `hacker move_rel stepper_id length` Move relative to current position
- `hacker move_abs stepper_id position` Move to absolute position

### Cutting jobs
To performe a cutting job execute 
```
hacker hack length amount
```
Length is the number of holes every cut piece has. Amount is the number of pieces to cut. Make sure to properly set the cutting position prior to executing a job.

