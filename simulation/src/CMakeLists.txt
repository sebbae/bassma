ADD_SUBDIRECTORY(examples)
#ADD_SUBDIRECTORY(gui)

set(SIMULATOR_SRCS
	IrrlichtSimulator.cpp
	IrrlichtRenderer.cpp
	OpenCVWebcam.cpp
)
add_lib(simulator.lib "${SIMULATOR_SRCS}")
add_exe(simulator simulator.cpp simulator.lib ${IRRLICHT_LIBRARIES} ${BULLET_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../media simulation)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
