PROJECT(simulation)

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
# OGRE                 #
########################
INCLUDE(cmake/OGRECMakeHelper.cmake)
find_package(OGRE REQUIRED)

########################
# OGRE                 #
########################
find_package(OIS REQUIRED)
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

########################
# Irrlicht             #
########################
find_package(Irrlicht REQUIRED)

########################
# OpenCV               #
########################
find_package(OpenCV REQUIRED)

########################
# Qt                   #
########################
SET(QT_USE_QTOPENGL TRUE)

find_package(Qt4 REQUIRED)
# defined by FIND_PACKAGE( Qt4 ) containing path to CMAKE
include(${QT_USE_FILE})

########################
# Boost                #
########################
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS thread date_time system REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
    ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
ENDIF()

########################
# OpenGL Glut          #
########################
find_package(OpenGL)
find_package(GLUT)

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
INCLUDE(cmake/CopyCmds.cmake)

# include CMAKE_BINARY_DIR for output of Qt's uic

include_directories(
	${Boost_INCLUDE_DIR}
	${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	${OGRE_Overlay_INCLUDE_DIRS}
	${IRRLICHT_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${GLUT_INCLUDE_DIRS}
	${CMAKE_BINARY_DIR}
)

MESSAGE(STATUS "IRRLICHT_INCLUDE_DIRS = ${IRRLICHT_INCLUDE_DIRS}")

########################
# Add files            #
########################
ADD_SUBDIRECTORY(src)
