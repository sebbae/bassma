set(OGRE_TUTORIAL_SRCS
	BaseApplication.cpp
)

add_lib(ogretutorial.lib "${OGRE_TUTORIAL_SRCS}")
add_exe(ogretutorial TutorialApplication.cpp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ogretutorial.lib)

copy_dir(${CMAKE_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR} ogretutorial)
copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../../media ogretutorial)