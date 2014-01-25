set(TUTORIAL_SRCS
	BaseApplication.cpp
)

add_lib(tutorial.lib "${TUTORIAL_SRCS}")
add_exe(tutorial TutorialApplication.cpp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES} tutorial.lib)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
