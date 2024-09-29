# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/pico-sdk")
set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/freertos-kernel")

set(PICO_BOARD "pico")
set(PICO_PLATFORM "rp2040")
set(PICO_COMPILER "pico_arm_cortex_m0plus_gcc")

include(pico_sdk_import.cmake)
include(freertos_kernel_import.cmake)


project(DemoFreeRTOS)

pico_sdk_init()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wfatal-errors -Wall -Wextra -Wconversion -Wshadow=local -Wdouble-promotion -Wformat=2 -Wformat-overflow=2 \
                -Wformat-nonliteral -Wformat-security -Wformat-truncation=2 -Wnull-dereference -Wimplicit-fallthrough=3 -Wshift-overflow=2         \
                -Wswitch-default -Wunused-parameter -Wunused-const-variable=2 -Wstrict-overflow=4 -Wstringop-overflow=3  -Wmissing-noreturn        \
                -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wmissing-format-attribute -Wsuggest-attribute=cold -Walloc-zero -Walloca    \
                -Wattribute-alias=2 -Wduplicated-branches -Wcast-qual -Wno-main")

add_executable(DemoFreeRTOS main.c)

include_directories(${CMAKE_SOURCE_DIR}/include ${FREERTOS_KERNEL_PATH}/include)

target_link_libraries(DemoFreeRTOS
    pico_stdlib
    hardware_gpio 
    FreeRTOS-Kernel-Heap4
)

pico_add_extra_outputs(DemoFreeRTOS)