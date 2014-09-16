set(SIMULATORGUI_MOC_HDRS
	QIrrlichtWidget.h
	QOpenCVWidget.h
	QOpenCVWebcamWidget.h
	QSimulation.h
	QSimulator.h
)

set(SIMULATORGUI_UIS
	QSimulation.ui
	QSimulator.ui
)

set(SIMULATORGUI_RCS
)

QT4_ADD_RESOURCES(SIMULATORGUI_RC_SRCS ${SIMULATORGUI_RCS})  
QT4_WRAP_UI(SIMULATORGUI_UI_HDRS ${SIMULATORGUI_UIS})
QT4_WRAP_CPP(SIMULATORGUI_MOC_SRCS ${SIMULATORGUI_MOC_HDRS})

set(SIMULATORGUI_SRCS
	QIrrlichtWidget.cpp
	QOpenCVWidget.cpp
	QOpenCVWebcamWidget.cpp
	QSimulation.cpp
	QSimulator.cpp
	${SIMULATORGUI_MOC_SRCS}
	${SIMULATORGUI_RC_SRCS}
	${SIMULATORGUI_UI_HDRS}
	${SIMULATOR_SRCS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_lib(simulatorgui.lib "${SIMULATORGUI_SRCS}")
add_exe(simulatorgui simulatorgui.cpp simulatorgui.lib simulator.lib ${IRRLICHT_LIBRARIES} ${BULLET_LIBRARIES} ${QT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

if(build_tests)
	include(test/CMakeLists.txt)
endif()
