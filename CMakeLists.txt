# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion RISCV_ZCB_RPI_2_1_1_3)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(pico_sdk_import.cmake)

project(pico_hello_usb C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_hello_usb
    main.c
    bme280_port.c
    bme280/bme280.c
)

target_include_directories(pico_hello_usb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/bme280
)

target_link_libraries(pico_hello_usb pico_stdlib hardware_i2c)

pico_enable_stdio_usb(pico_hello_usb 1)
pico_enable_stdio_uart(pico_hello_usb 0)

pico_add_extra_outputs(pico_hello_usb)
