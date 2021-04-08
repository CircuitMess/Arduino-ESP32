cmake_minimum_required(VERSION 3.15)
project(Spencer-Library)
set(PORT /dev/ttyUSB0)
set(DEVICE cm:esp32:spencer)
set(CMAKE_CXX_STANDARD 11)

if(WIN32 OR CYGWIN)
    set(Arduino15 $ENV{APPDATA}/../Local/Arduino15)
    set(ArduinoLibs $ENV{USERPROFILE}/Documents/Arduino/libraries)

    string(REPLACE "\\" "/" Arduino15 ${Arduino15})
    string(REPLACE "\\" "/" ArduinoLibs ${ArduinoLibs})

    if(${CYGWIN})
        string(REGEX REPLACE "^([A-Za-z]):" "/cygdrive/\\1" Arduino15 ${Arduino15})
        string(REGEX REPLACE "^([A-Za-z]):" "/cygdrive/\\1" ArduinoLibs ${ArduinoLibs})
    endif()
else()
    set(Arduino15 ~/.arduino15)
    set(ArduinoLibs ~/Arduino/libraries)
endif()

include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/Wire/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/FS/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/SD/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/SPI/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/WiFi/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/SPIFFS/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/HTTPClient/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/WebServer/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/DNSServer/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/CircuitOS/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/TFT_eSPI)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/TFT_eSPI/Extensions)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/ESP8266Audio/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/SerialFlash)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/ArduinoJson/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/NTPClient)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/libraries/ESP32Ping)

include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/cores/esp32)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk/include)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk/include/esp32)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk/include/freertos)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk/include/driver)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk/include/driver/driver)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.1.0/tools/sdk/include/spi_flash)


AUX_SOURCE_DIRECTORY(src SOURCES)
AUX_SOURCE_DIRECTORY(src/Audio SOURCES)
AUX_SOURCE_DIRECTORY(src/DataStream SOURCES)
AUX_SOURCE_DIRECTORY(src/Speech SOURCES)
AUX_SOURCE_DIRECTORY(src/Util SOURCES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_definitions(-DDEBUG)
add_definitions(-DESP32)
add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DCIRCUITOS_FREERTOS)
add_definitions(-DCIRCUITOS_NVS)
add_definitions(-DCIRCUITOS_LEDMATRIX)
add_definitions(-DCIRCUITOS_SERIALFLASH)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_PROJECT_NAME}.ino PROPERTIES LANGUAGE CXX)

add_custom_command(OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.ino.elf
        COMMAND arduino-cli compile --fqbn ${DEVICE} --output-dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_PROJECT_NAME}.ino
        DEPENDS ${CMAKE_PROJECT_NAME}.ino ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(CMBuild
        SOURCES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.ino.elf)

add_custom_target(CMUpload ALL
        COMMAND arduino-cli upload --fqbn ${DEVICE} -p ${PORT} --input-dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        DEPENDS CMBuild
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(${CMAKE_PROJECT_NAME}.bin ${CMAKE_PROJECT_NAME}.ino ${SOURCES})