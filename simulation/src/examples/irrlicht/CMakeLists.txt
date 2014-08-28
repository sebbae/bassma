add_exe(irrlichttutorial IrrlichtQuake3Map.cpp ${IRRLICHT_LIBRARIES})

copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../media irrlichttutorial)