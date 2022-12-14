cmake_minimum_required(VERSION 3.14)

project(livekeys LANGUAGES C CXX VERSION 1.9.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES #[[Qt6 ]] Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Xml Svg WebEngine REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Path Setup

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(BINARY_PATH  ${CMAKE_BINARY_DIR}/bin)

if (APPLE)
    set(DEPLOY_PATH          ${BINARY_PATH}/livekeys.app/Contents)
    set(LIBRARY_DEPLOY_PATH  ${DEPLOY_PATH}/Frameworks/Live.framework/Libraries)
    set(MODULE_DEPLOY_PATH   ${DEPLOY_PATH}/PlugIns)
else()
    set(DEPLOY_PATH          ${BINARY_PATH}/bin)
    set(LIBRARY_DEPLOY_PATH  ${DEPLOY_PATH})
    set(MODULE_DEPLOY_PATH   ${DEPLOY_PATH}/plugins)
endif()

# Release or Debug Setup

set(DEBUG_BUILD   FALSE)
set(RELEASE_BUILD FALSE)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG_BUILD TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(DEBUG_BUILD TRUE)
else()
    set(RELEASE_BUILD TRUE)
endif()


# Configuration options

option(BUILD_EXECUTABLE        "Build livekeys executable."        ON)
option(BUILD_CV                "Build computer vision modules."    ON)
option(BUILD_TUTORIALS         "Build user tutorials."             ON)
option(BUILD_ELEMENTS_COMPILER "Build compiler for live elements." ON)
option(BUILD_ELEMENTS_ENGINE   "Build engine for live elements."   ON)
option(BUILD_PACKAGE_SAMPLES   "Build samples for each package."   ON)
option(BUILD_TESTS             "Build tests."                      ON)
option(BUILD_UNIX_BFD_TRACE    "Unix: Build with bfd stack trace." ON)
option(BUILD_QML_PLUGINTYPES   "Enable qml plugintypes output."    OFF)


# Live Elements Configuration

set(SHOULD_BUILD_ELEMENTS OFF)
set(SHOULD_BUILD_ELEMENTS_ENGINE ${BUILD_ELEMENTS_ENGINE})

if(BUILD_ELEMENTS_ENGINE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/project/3rdparty/v8.cmake)
    if(NOT V8_FOUND)
        message("CONFIG: V8 package not found. Skipping live elements engine.")
        set(SHOULD_BUILD_ELEMENTS_ENGINE OFF)
    else()
        set(BUILD_ELEMENTS_COMPILER ON)
    endif()
endif()

if(BUILD_ELEMENTS_COMPILER)
    set(SHOULD_BUILD_ELEMENTS ON)
endif()

# Computer Vision Configuration

set(SHOULD_BUILD_CV ${BUILD_CV})
if(SHOULD_BUILD_CV)
    find_package(OpenCV)
    if(NOT OPENCV_FOUND)
        message("CONFIG: OpenCV package not found. Skipping computer vision modules.")
        set(SHOULD_BUILD_CV OFF)
    endif()
endif()

# Configuragion Log

message("\nBuild Configuration:")
message("  * BUILD_EXECUTABLE:        ${BUILD_EXECUTABLE}")
message("  * BUILD_TUTORIALS:         ${BUILD_TUTORIALS}")
message("  * BUILD_ELEMENTS_COMPILER: ${BUILD_ELEMENTS_COMPILER}")
message("  * BUILD_ELEMENTS_ENGINE:   ${SHOULD_BUILD_ELEMENTS_ENGINE}")
message("  * BUILD_CV:                ${SHOULD_BUILD_CV}")
message("  * BUILD_PACKAGE_SAMPLES:   ${BUILD_PACKAGE_SAMPLES}")
message("  * BUILD_TESTS:             ${BUILD_TESTS}")
message("")

include(${CMAKE_CURRENT_SOURCE_DIR}/project/functions.cmake)

add_subdirectory(lib)
add_subdirectory(plugins)

if(BUILD_EXECUTABLE)
    add_subdirectory(application)
endif()

if(BUILD_TESTS)
    find_package(QT NAMES #[[Qt6 ]] Qt5 COMPONENTS Test REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)
    set(TESTS_DEPLOY_PATH  ${CMAKE_BINARY_DIR}/tests)
    add_subdirectory(tests)
endif()
