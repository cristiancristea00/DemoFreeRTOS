cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD "pico")
set(PICO_PLATFORM "rp2040")
set(PICO_COMPILER "pico_arm_cortex_m0plus_gcc")

include(pico-sdk/pico_sdk_init.cmake)

project(DemoFreeRTOS)

pico_sdk_init()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wfatal-errors -Wall -Wextra -Wconversion -Wshadow=local -Wdouble-promotion -Wformat=2 -Wformat-overflow=2 \
                -Wformat-nonliteral -Wformat-security -Wformat-truncation=2 -Wnull-dereference -Wimplicit-fallthrough=3 -Wshift-overflow=2                \
                -Wswitch-default -Wunused-parameter -Wunused-const-variable=2 -Wstrict-overflow=4 -Wstringop-overflow=3  -Wmissing-noreturn               \
                -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wmissing-format-attribute -Wsuggest-attribute=cold -Walloc-zero -Walloca           \
                -Wattribute-alias=2 -Wduplicated-branches -Wcast-qual -Wno-main")

add_executable(DemoFreeRTOS main.c)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE ${CMAKE_SOURCE_DIR})

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_ARM_CM0" CACHE STRING "" FORCE)

add_subdirectory(freertos-kernel)

target_link_libraries(DemoFreeRTOS
    pico_stdlib
    hardware_gpio 
    freertos_kernel
    freertos_config
)

pico_add_extra_outputs(DemoFreeRTOS)