cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(ti_mspm0_libxr_dev LANGUAGES C CXX ASM)

option(USE_MSPM0_TEMPLATE_LIBXR_CMAKE "Include cmake/LibXR.CMake for MSPM0 SDK integration" OFF)
if(USE_MSPM0_TEMPLATE_LIBXR_CMAKE)
  include("${CMAKE_SOURCE_DIR}/cmake/LibXR.CMake")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MSPM0_SDK_ROOT "${CMAKE_SOURCE_DIR}/mspm0-sdk" CACHE PATH "Path to MSPM0 SDK root")
set(SYSCFG_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/sysconfig")

set(SYSCFG_REQUIRED_FILES
    "${SYSCFG_OUTPUT_DIR}/device_linker.lds"
    "${SYSCFG_OUTPUT_DIR}/device.opt"
    "${SYSCFG_OUTPUT_DIR}/device.lds.genlibs"
    "${SYSCFG_OUTPUT_DIR}/ti_msp_dl_config.c"
    "${SYSCFG_OUTPUT_DIR}/ti_msp_dl_config.h")

foreach(_syscfg_file IN LISTS SYSCFG_REQUIRED_FILES)
  if(NOT EXISTS "${_syscfg_file}")
    message(FATAL_ERROR "Required SysConfig output file missing: ${_syscfg_file}")
  endif()
endforeach()

set(APP_SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.c"
    "${CMAKE_SOURCE_DIR}/src/app_main.cpp"
    "${CMAKE_SOURCE_DIR}/mspm0-sdk/examples/nortos/LP_MSPM0G3507/driverlib/empty_driverlib_library/gcc/startup_mspm0g350x_gcc.c"
    "${CMAKE_SOURCE_DIR}/libxr/src/core/libxr_assert.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/src/core/libxr_type.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/src/core/libxr_mem.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/src/core/libxr_time.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/src/system/timer.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/src/structure/lockfree_list.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/src/structure/double_buffer.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/driver/mspm0/mspm0_gpio.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/driver/mspm0/mspm0_atomic_shim.c"
    "${CMAKE_SOURCE_DIR}/libxr/driver/mspm0/mspm0_timebase.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/system/None/semaphore.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/system/None/thread.cpp"
    "${CMAKE_SOURCE_DIR}/libxr/system/None/libxr_system.cpp"
    "${SYSCFG_OUTPUT_DIR}/ti_msp_dl_config.c")

add_executable(${PROJECT_NAME}.elf ${APP_SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${SYSCFG_OUTPUT_DIR}"
    "${MSPM0_SDK_ROOT}/source"
    "${MSPM0_SDK_ROOT}/source/ti/devices/msp"
    "${MSPM0_SDK_ROOT}/source/third_party/CMSIS/Core/Include"
    "${MSPM0_SDK_ROOT}/source/ti/driverlib"
    "${MSPM0_SDK_ROOT}/source/ti/driverlib/m0p"
    "${CMAKE_SOURCE_DIR}/libxr/src"
    "${CMAKE_SOURCE_DIR}/libxr/src/core"
    "${CMAKE_SOURCE_DIR}/libxr/src/system"
    "${CMAKE_SOURCE_DIR}/libxr/src/utils"
    "${CMAKE_SOURCE_DIR}/libxr/src/structure"
    "${CMAKE_SOURCE_DIR}/libxr/src/middleware"
    "${CMAKE_SOURCE_DIR}/libxr/src/driver"
    "${CMAKE_SOURCE_DIR}/libxr/driver/mspm0"
    "${CMAKE_SOURCE_DIR}/libxr/system/None")

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    LIBXR_DEFAULT_SCALAR=double
    LIBXR_LOG_LEVEL=4
    LIBXR_LOG_OUTPUT_LEVEL=4
    LIBXR_NOT_SUPPORT_MUTI_THREAD=1
    LIBXR_NO_EIGEN
    LIBXR_PRINTF_BUFFER_SIZE=128
    XR_LOG_MESSAGE_MAX_LEN=64
    __MSPM0G3507__
    __USE_SYSCONFIG__)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
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

set(DRIVERLIB_A "${MSPM0_SDK_ROOT}/source/ti/driverlib/lib/gcc/m0p/mspm0g1x0x_g3x0x/driverlib.a")

target_link_libraries(${PROJECT_NAME}.elf PRIVATE "${DRIVERLIB_A}")
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    "-Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map"
    "-T${SYSCFG_OUTPUT_DIR}/device_linker.lds")

find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND "${ARM_OBJCOPY}" -O ihex "$<TARGET_FILE:${PROJECT_NAME}.elf>" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex"
    COMMAND "${ARM_OBJCOPY}" -O binary "$<TARGET_FILE:${PROJECT_NAME}.elf>" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin"
    COMMENT "Generating HEX/BIN images")
