set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to your local LibXR repository.
set(LIBXR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libxr" CACHE PATH "Path to LibXR source tree")
set(MSPM0_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/mspm0-sdk" CACHE PATH "Path to MSPM0 SDK root")

if(NOT EXISTS "${LIBXR_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "LibXR not found. Please set LIBXR_DIR correctly: ${LIBXR_DIR}")
endif()

# LibXR platform/driver selection
set(LIBXR_SYSTEM None)
# Prefer MSPM0 driver when LibXR provides a CMake entry for it.
if(EXISTS "${LIBXR_DIR}/driver/mspm0/CMakeLists.txt")
    set(LIBXR_DRIVER mspm0)
else()
    unset(LIBXR_DRIVER)
endif()
set(LIBXR_NO_EIGEN True)

# Optional XRobot modules
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules")
    set(XROBOT_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Modules")
endif()

# Import LibXR as a subproject
add_subdirectory("${LIBXR_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/libxr")

# Make LibXR compile with the same MSPM0 SDK include/link settings.
if(TARGET xr AND EXISTS "${MSPM0_SDK_ROOT}/source")
    target_include_directories(xr PUBLIC
        "${MSPM0_SDK_ROOT}/source"
        "${MSPM0_SDK_ROOT}/source/ti/devices/msp"
        "${MSPM0_SDK_ROOT}/source/third_party/CMSIS/Core/Include"
        "${MSPM0_SDK_ROOT}/source/ti/driverlib"
        "${MSPM0_SDK_ROOT}/source/ti/driverlib/m0p"
        "${LIBXR_DIR}/driver/mspm0")

    set(_MSPM0_DRIVERLIB_A "${MSPM0_SDK_ROOT}/source/ti/driverlib/lib/gcc/m0p/mspm0g1x0x_g3x0x/driverlib.a")
    if(EXISTS "${_MSPM0_DRIVERLIB_A}")
        target_link_libraries(xr PUBLIC "${_MSPM0_DRIVERLIB_A}")
    endif()
endif()

# Let app sources directly include LibXR headers.
if(TARGET app)
    target_link_libraries(app PUBLIC xr)
endif()

# Ensure LibXR object files are linked into the final ELF target.
if(DEFINED APP_ELF_NAME AND TARGET ${APP_ELF_NAME})
    target_link_libraries(${APP_ELF_NAME} xr)
endif()

# Keep app in debug mode while shrinking LibXR binary size.
if(TARGET xr)
    string(TOLOWER "${CMAKE_BUILD_TYPE}" _libxr_build_type)
    if(_libxr_build_type STREQUAL "debug")
        target_compile_options(xr PRIVATE -O0)
    endif()
endif()
