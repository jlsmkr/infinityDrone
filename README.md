# Infinity Drone

An atmospheric, high-fidelity drone synth for the __Daisy Seed__. It layers several sounds to create infinite drones that appear to move and evolve over time. It is designed to fit into a guitar pedal case with an OLED display and five foot switches.

## Getting Started
### 1. Daisy Seed
The __Daisy Seed__ is the core of this project. It has a powerful digital sound processor (DSP) and is pretty affordable. For this project, we're using the version with 65 MB SSDRAM. If you're living in North America, you can get it here: https://electro-smith.com/products/daisy-seed, if you're in Europe, you can get it here: https://www.electrokit.com/en/electrosmith-daisy-seed-embedded-dsp-platform. Make sure you choose the right version (the one with the extra chip).

We are using the Arduino IDE for programming. To get it running, you can follow this tutorial: https://www.youtube.com/watch?v=UyQWK8JFTps.

### 2. Display
We are using this display: https://www.otronic.nl/en/24-inch-oled-display-128x64-i2c-ssd1309-33v.html. But you can use whatever display you like most. Just make sure it supports I2C and has a resolution of 128x64.

### 3. Other hardware
We are using a breadboard to connect all components. In addition to the Daisy and the display, you will also need five switches, some jumper wires and an audio cable. We recommend using an audiio interface to capture the sound of the Daisy.

## Pinout
```
                           ______________________________
                          |                              |
                          |  1                        40 |  GND
                          |  2                        39 |
                          |  3                        38 |
                          |  4                        37 |
                          |  5                        36 |
                          |  6                        35 |
                          |  7                        34 |
                          |  8                        33 |
                          |  9                        32 |
                          | 10                        31 |
                          | 11                        30 |
                          | 12                        29 |
                          | 13                        28 |
                          | 14                        27 |
                          | 15                        26 |  SWITCH 5 -> GND
                          | 16                        25 |  SWITCH 4 -> GND
                          | 17                        24 |  SWITCH 3 -> GND
Interface <- AUDIO OUT 1  | 18                        23 |  SWITCH 2 -> GND
Interface <- AUDIO OUT 2  | 19                        22 |  SWITCH 1 -> GND
                     GND  | 20                        21 |
                          |______________________________|
```
