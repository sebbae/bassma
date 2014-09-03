add_exe(irrlichttutorial IrrlichtQuake3Map.cpp ${IRRLICHT_LIBRARIES})

add_exe(irrlichtbullet IrrlichtBullet.cpp ${IRRLICHT_LIBRARIES} ${BULLET_LIBRARIES})
add_exe(irrlichtbullet2 IrrlichtBullet2.cpp ${IRRLICHT_LIBRARIES} ${BULLET_LIBRARIES})



copy_dir(${CMAKE_SOURCE_DIR}/media ${CMAKE_CURRENT_BINARY_DIR}/../media irrlichttutorial)