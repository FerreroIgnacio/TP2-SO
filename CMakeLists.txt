cmake_minimum_required(VERSION 3.10)
project(FullSystem C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define include directories more comprehensively
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Bootloader
        ${CMAKE_SOURCE_DIR}/Bootloader/include
        ${CMAKE_SOURCE_DIR}/Kernel
        ${CMAKE_SOURCE_DIR}/Kernel/include
        ${CMAKE_SOURCE_DIR}/Userland
        ${CMAKE_SOURCE_DIR}/Userland/include
        ${CMAKE_SOURCE_DIR}/Userland/SampleCodeModule/include
        ${CMAKE_SOURCE_DIR}/Image
        ${CMAKE_SOURCE_DIR}/Image/include
)

# Collect all source files recursively
file(GLOB_RECURSE ALL_SOURCES
        "${CMAKE_SOURCE_DIR}/Bootloader/*.c"
        "${CMAKE_SOURCE_DIR}/Bootloader/*.h"
        "${CMAKE_SOURCE_DIR}/Bootloader/*.asm"
        "${CMAKE_SOURCE_DIR}/Bootloader/*.s"
        "${CMAKE_SOURCE_DIR}/Kernel/*.c"
        "${CMAKE_SOURCE_DIR}/Kernel/*.h"
        "${CMAKE_SOURCE_DIR}/Kernel/*.asm"
        "${CMAKE_SOURCE_DIR}/Kernel/*.s"
        "${CMAKE_SOURCE_DIR}/Userland/*.c"
        "${CMAKE_SOURCE_DIR}/Userland/*.h"
        "${CMAKE_SOURCE_DIR}/Userland/*.asm"
        "${CMAKE_SOURCE_DIR}/Userland/*.s"
        "${CMAKE_SOURCE_DIR}/Image/*.c"
        "${CMAKE_SOURCE_DIR}/Image/*.h"
        "${CMAKE_SOURCE_DIR}/Image/*.asm"
        "${CMAKE_SOURCE_DIR}/Image/*.s"
)

# Create a single comprehensive target for indexing
add_library(fullsystem_ide EXCLUDE_FROM_ALL ${ALL_SOURCES})

# Set compiler definitions that might be used in your OS code
target_compile_definitions(fullsystem_ide PRIVATE
        __KERNEL__
        __OS__
)

# Set properties for better indexing
set_target_properties(fullsystem_ide PROPERTIES
        C_STANDARD 99
        LINKER_LANGUAGE C
)

# Disable problematic flags for indexing
target_compile_options(fullsystem_ide PRIVATE
        -Wno-builtin-declaration-mismatch
        -Wno-implicit-function-declaration
        -fno-strict-aliasing
)

# Create separate targets for each component (for organization)
file(GLOB_RECURSE BOOTLOADER_SOURCES "${CMAKE_SOURCE_DIR}/Bootloader/*.[ch]" "${CMAKE_SOURCE_DIR}/Bootloader/*.asm")
file(GLOB_RECURSE KERNEL_SOURCES "${CMAKE_SOURCE_DIR}/Kernel/*.[ch]" "${CMAKE_SOURCE_DIR}/Kernel/*.asm")
file(GLOB_RECURSE USERLAND_SOURCES "${CMAKE_SOURCE_DIR}/Userland/*.[ch]" "${CMAKE_SOURCE_DIR}/Userland/*.asm")

if(BOOTLOADER_SOURCES)
    add_library(bootloader_ide EXCLUDE_FROM_ALL ${BOOTLOADER_SOURCES})
    target_include_directories(bootloader_ide PRIVATE ${CMAKE_SOURCE_DIR}/Bootloader/include)
endif()

if(KERNEL_SOURCES)
    add_library(kernel_ide EXCLUDE_FROM_ALL ${KERNEL_SOURCES})
    target_include_directories(kernel_ide PRIVATE ${CMAKE_SOURCE_DIR}/Kernel/include)
endif()

if(USERLAND_SOURCES)
    add_library(userland_ide EXCLUDE_FROM_ALL ${USERLAND_SOURCES})
    target_include_directories(userland_ide PRIVATE
            ${CMAKE_SOURCE_DIR}/Userland/include
            ${CMAKE_SOURCE_DIR}/Userland/SampleCodeModule/include
    )
endif()

# Build targets using your existing Makefiles
add_custom_target(bootloader
        COMMAND ${CMAKE_MAKE_PROGRAM} all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Bootloader
        COMMENT "Building bootloader"
)

add_custom_target(kernel
        COMMAND ${CMAKE_MAKE_PROGRAM} all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Kernel
        COMMENT "Building kernel"
)

add_custom_target(userland
        COMMAND ${CMAKE_MAKE_PROGRAM} all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Userland
        COMMENT "Building userland"
)

add_custom_target(image
        COMMAND ${CMAKE_MAKE_PROGRAM} all
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Image
        DEPENDS bootloader kernel userland
        COMMENT "Creating system image"
)

add_custom_target(build-all ALL
        DEPENDS bootloader kernel userland image
)

add_custom_target(clean-all
        COMMAND ${CMAKE_MAKE_PROGRAM} clean
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Bootloader
        COMMAND ${CMAKE_MAKE_PROGRAM} clean
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Image
        COMMAND ${CMAKE_MAKE_PROGRAM} clean
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Kernel
        COMMAND ${CMAKE_MAKE_PROGRAM} clean
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Userland
        COMMENT "Cleaning all components"
)