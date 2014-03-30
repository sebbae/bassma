ADD_SUBDIRECTORY(irrlicht)
ADD_SUBDIRECTORY(ogre)

add_exe(simulation simulation.cpp ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

set(SIMULATOR_SRCS
	IrrlichtSimulator.cpp
)
add_lib(simulator.lib "${SIMULATOR_SRCS}")
add_exe(simulator simulator.cpp simulator.lib ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

add_exe(main main.cpp ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})


copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../media simulation)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
