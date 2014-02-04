ADD_SUBDIRECTORY(irrlicht)
ADD_SUBDIRECTORY(ogre)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
