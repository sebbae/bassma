PROJECT(vision)

########################
# General              #
########################
# minimum CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

MESSAGE("SOURCE DIR IS " ${CMAKE_SOURCE_DIR})

# prohibit in-source-builds
IF (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    MESSAGE(FATAL_ERROR "In-source-builds are not allowed.")
ENDIF (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

# detect OS type
IF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    SET(LINUX "${CMAKE_SYSTEM}")
ENDIF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")

# set module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

########################
# Options              #
########################
OPTION(build_tests "Build unit tests" OFF)

########################
# Compilation          #
########################
# enable C++11 features
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3 ${CMAKE_CXX_FLAGS}")

########################
# OpenCV               #
########################
find_package(OpenCV REQUIRED)

########################
# Boost                #
########################
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS thread date_time REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

########################
# Testing              #
########################
IF(build_tests)
	enable_testing()
	find_package(Threads)
	find_package(Gtest)
ENDIF()

########################
# Macros               #
########################
INCLUDE(cmake/Cmds.cmake)
INCLUDE(cmake/GTestCmds.cmake)

########################
# Add files            #
########################
ADD_SUBDIRECTORY(src)
