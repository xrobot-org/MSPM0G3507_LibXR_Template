cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(ti_mspm0_libxr_dev LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("${CMAKE_SOURCE_DIR}/cmake/LibXR.CMake")

set(APP_SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.c"
    "${CMAKE_SOURCE_DIR}/src/app_main.cpp"
    "${MSPM0_STARTUP_FILE}"
    "${SYSCFG_C_FILE}")

add_executable(${PROJECT_NAME}.elf ${APP_SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    ${MSPM0_COMMON_INCLUDE_DIRS})

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    ${MSPM0_COMMON_COMPILE_DEFINITIONS})

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${MSPM0_ARCH_COMPILE_OPTIONS}
    -ffunction-sections
    -fdata-sections
    -Og
    -g
    "@${SYSCFG_OUTPUT_DIR}/device.opt"
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE xr)
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    "-Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map"
    "-T${SYSCFG_LINKER_FILE}")

find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND "${ARM_OBJCOPY}" -O ihex "$<TARGET_FILE:${PROJECT_NAME}.elf>" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex"
    COMMAND "${ARM_OBJCOPY}" -O binary "$<TARGET_FILE:${PROJECT_NAME}.elf>" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin"
    COMMENT "Generating HEX/BIN images")
