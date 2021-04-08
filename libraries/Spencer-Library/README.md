# Spencer Library
> Arduino library for programming Spencer, your DIY voice assistant.


Spencer is a DIY voice assistant that talks, lights-up, connects to the internet, and understands simple voice commands. You can learn more [here](https://circuitmess.com/spencer/).

Spencer is also a part of [CircuitMess STEM Box](https://igg.me/at/stem-box/x#/) - a series of fun electronic kits to help children and adults understand the basics of technologies everybody's talking about.


![](https://circuitmess.com/wp-content/uploads/2020/11/spencer-page-1.jpg)

## Installation

TODO - add Arduino library manager installation.


## Development setup

The library uses several dependency libraries:
- [CircuitOS](https://github.com/CircuitMess/CircuitOS) by CircuitMess
- [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio) by Earle F. Philhower, III
- [SerialFlash](https://github.com/PaulStoffregen/SerialFlash) by Paul Stoffregen
- [ESP32Ping](https://github.com/marian-craciunescu/ESP32Ping) by Marian Craciunescu
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoît Blanchon
- [NTPClient](https://github.com/taranais/NTPClient) by taranais

You can download all of these libraries by downloading or cloning the repository into your Documents/Arduino/Libraries folder on Windows, or ~/Arduino/libraries folder on Linux.

If you prefer, you can also follow the installation instructions on the respective repository pages.

To run a test compilation you need to have [CMake](https://cmake.org/) and [arduino-cli](https://github.com/arduino/arduino-cli)  installed. You also need to have both of them registered in the PATH.

In the CMakeLists.txt file change the port to your desired COM port (default is /dev/ttyUSB0):
```
set(PORT /dev/ttyUSB0)
```
Then in the root directory of the repository type:
```
mkdir build
cd build
cmake ..
cmake --build . --target CMUpload
```
This should compile and upload a test sketch.
## Used libraries and copyright notices
[See NOTICE](https://github.com/CircuitMess/Spencer-Library/blob/master/NOTICE.md)

## Meta


**CircuitMess**  - https://circuitmess.com/

**Facebook** - https://www.facebook.com/thecircuitmess/

**Instagram** - https://www.instagram.com/thecircuitmess/

**Twitter** - https://twitter.com/circuitmess 

**YouTube** - https://www.youtube.com/channel/UCVUvt1CeoZpCSnwg3oBMsOQ

----
Copyright © 2020 CircuitMess

Licensed under [GPLv3 License](https://www.gnu.org/licenses/gpl-3.0.html).
