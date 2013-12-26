set(VISION_SRC
)

add_lib(vision.lib "${VISION_SRCS}")
add_exe(simple simple.cpp ${OpenCV_LIBS} vision.lib)
add_exe(facedetect facedetect.cpp ${OpenCV_LIBS} vision.lib)
add_exe(facedetect2 facedetect2.cpp ${OpenCV_LIBS} vision.lib)

if(build_tests)
	include(test/CMakeLists.txt)
endif()
