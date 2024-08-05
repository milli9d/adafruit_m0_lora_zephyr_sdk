# boilerplate cmake
cmake_minimum_required(VERSION 3.20.0)

list(APPEND DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/usb.overlay)

# import zephyr package; this is required for ZephyrOS cmake system to work
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(m0_lora)

file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*)

target_sources(app PRIVATE ${srcs})
