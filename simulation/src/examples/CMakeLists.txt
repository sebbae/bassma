ADD_SUBDIRECTORY(irrlicht)
#ADD_SUBDIRECTORY(ogre)
#ADD_SUBDIRECTORY(qt)

add_exe(simulation simulation.cpp ${IRRLICHT_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

add_exe(main main.cpp ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})