ADD_SUBDIRECTORY(irrlicht)
ADD_SUBDIRECTORY(ogre)
ADD_SUBDIRECTORY(qt)

add_exe(simulation simulation.cpp ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

set(SIMULATOR_SRCS
	IrrlichtSimulator.cpp
)
add_lib(simulator.lib "${SIMULATOR_SRCS}")
add_exe(simulator simulator.cpp simulator.lib ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

add_exe(main main.cpp ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

set(SIMULATORGUI_MOC_HDRS
	QIrrlichtWidget.h
	QOpenCVWidget.h
	QSimulator.h
)

set(SIMULATORGUI_UIS
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
	QSimulator.cpp
	${SIMULATORGUI_MOC_SRCS}
	${SIMULATORGUI_RC_SRCS}
	${SIMULATORGUI_UI_HDRS}
	${SIMULATOR_SRCS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_lib(simulatorgui.lib "${SIMULATORGUI_SRCS}")
add_exe(simulatorgui simulatorgui.cpp simulatorgui.lib ${QT_LIBRARIES} ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})


copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../media simulation)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
