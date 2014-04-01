set(QTEXAMPLE_MOC_HDRS
	cqtopencvviewergl.h
	qtglwebcamdemo.h
)

set(QTEXAMPLE_UIS
	qtglwebcamdemo.ui
)

set(QTEXAMPLE_RCS
)

QT4_ADD_RESOURCES(QTEXAMPLE_RC_SRCS ${QTEXAMPLE_RCS})  
QT4_WRAP_UI(QTEXAMPLE_UI_HDRS ${QTEXAMPLE_UIS})
QT4_WRAP_CPP(QTEXAMPLE_MOC_SRCS ${QTEXAMPLE_MOC_HDRS})

set(QTEXAMPLE_SRCS
	cqtopencvviewergl.cpp
	qtglwebcamdemo.cpp
	${QTEXAMPLE_MOC_SRCS}
	${QTEXAMPLE_RC_SRCS}
	${QTEXAMPLE_UI_HDRS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_lib(qtexample.lib "${QTEXAMPLE_SRCS}")
add_exe(qtexample qtexample.cpp qtexample.lib ${QT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

if(build_tests)
	#include(test/CMakeLists.txt)
endif()
