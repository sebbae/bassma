ADD_SUBDIRECTORY(irrlicht)
ADD_SUBDIRECTORY(ogre)

add_exe(simulation simulation.cpp ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../media simulation)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
