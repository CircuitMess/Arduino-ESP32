# Wheelson Library
> Arduino library for programming Wheelson, your own AI self-driving car.


Autonomous cars are the future and we’ll show you how it works.

This tiny wheeled robot has a camera and a microcomputer and can be programmed to autonomously navigate a small road while driving, just like an autonomous car would.

Jay-D is also a part of [CircuitMess STEM Box](https://igg.me/at/stem-box/x#/) - a series of fun electronic kits to help children and adults understand the basics of technologies everybody's talking about.


![](https://circuitmess.com/wp-content/uploads/2021/05/wheelson-new-2.jpg)

# Installation

The library is automatically installed when you install the CircuitMess ESP32 Arduino platform, which contains the Jay-D board. More info on [CircuitMess/Arduino-Packages](https://github.com/CircuitMess/Arduino-Packages).

# Development setup

The library uses several dependency libraries:
- [CircuitOS](https://github.com/CircuitMess/CircuitOS) by CircuitMess
- [OpenCV](https://github.com/opencv/opencv) by the OpenCV team (precompiled)

These libraries are automatically installed when you install the CircuitMess ESP32 Arduino platform.

## Using Arduino IDE

Simply open Wheelson-Library.ino using Arduino IDE, set the board to Wheelson, and compile.

## Using CMake

To run a test compilation you need to have [CMake](https://cmake.org/) and [arduino-cli](https://github.com/arduino/arduino-cli)  installed. You also need to have both of them registered in the PATH.

In the CMakeLists.txt file change the port to your desired COM port (default is /dev/ttyUSB0):
```
set(PORT /dev/ttyUSB0)
```
Then in the root directory of the repository type:
```
mkdir cmake
cd cmake
cmake ..
cmake --build . --target CMBuild
```
This will compile the binaries, and place the .bin and .elf files in the build/ directory located in the root of the repository.

To compile the binary, and upload it according to the port set in CMakeLists.txt, run

```cmake --build . --target CMBuild```

in the cmake directory.
# Used libraries and copyright notices
[See NOTICE](https://github.com/CircuitMess/Wheelson-Library/blob/master/NOTICE.md)

Code that runs on the Nuvoton N76E616 chip is located in a [separate repo](https://github.com/CircuitMess/Wheelson-Nuvoton-Firmware).

# Meta


**CircuitMess**  - https://circuitmess.com/

**Facebook** - https://www.facebook.com/thecircuitmess/

**Instagram** - https://www.instagram.com/thecircuitmess/

**Twitter** - https://twitter.com/circuitmess

**YouTube** - https://www.youtube.com/channel/UCVUvt1CeoZpCSnwg3oBMsOQ

----
Copyright © 2021 CircuitMess

Licensed under [MIT License](https://opensource.org/licenses/MIT).