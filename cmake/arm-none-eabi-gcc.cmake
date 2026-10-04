# Toolchain file for the GNU Arm Embedded toolchain (arm-none-eabi-gcc).
# The CPU options (Cortex-M0+) are set by the project in CMakeLists.txt and cmake/LibXR.CMake.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# arm-none-eabi- must be part of PATH, or TOOLCHAIN_PREFIX is a full path prefix such as
# /opt/arm-gnu-toolchain/bin/arm-none-eabi-
set(TOOLCHAIN_PREFIX "arm-none-eabi-" CACHE STRING "Cross-compiler prefix")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

# The compiler checks link a static library, since no linker script is available yet.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
