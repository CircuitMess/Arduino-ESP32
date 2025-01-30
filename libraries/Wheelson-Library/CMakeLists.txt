cmake_minimum_required(VERSION 3.15)
project(Wheelson-Library)
set(PORT /dev/ttyS5)
set(DEVICE cm:esp32:wheelson)
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

include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/Wire/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/FS/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/SD/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/SPI/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/WiFi/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/SPIFFS/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/HTTPClient/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/WebServer/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/DNSServer/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/CircuitOS/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/LovyanGFX/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/FDK-AAC/src)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/libraries/OpenCV-ESP32/src)

include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/cores/esp32)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include/esp32)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include/freertos)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include/driver)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include/driver/driver)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include/spi_flash)
include_directories(${Arduino15}/packages/cm/hardware/esp32/1.7.3/tools/sdk/include/esp32-camera/)

include_directories(${ArduinoLibs}/OpenCV-ESP32/src)



file(GLOB_RECURSE SOURCES "src/**.cpp" "src/**.c" "src/**.hpp" "src/**.h")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_definitions(-DDEBUG)
add_definitions(-DESP32)
add_definitions(-DCIRCUITOS_LOVYANGFX)
add_definitions(-DLOVYAN_MANUAL)

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