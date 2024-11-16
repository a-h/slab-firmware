cmake_minimum_required(VERSION 3.12)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(external/pico-sdk/pico_sdk_init.cmake)

project(slab C CXX ASM) # Name of project, can have multiple targets.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Create files for CCLS to use for code completion.
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Require Pico SDK version 1.3.0
if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

# Add compiler flags:
add_compile_options(-Wall # Enable all warnings
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Og) # Enable debug optimizations
else()
        add_compile_options(-O3) # Enable speed optimizations
endif()


#add_subdirectory(left)
add_subdirectory(right)
#add_subdirectory(hackpad)
