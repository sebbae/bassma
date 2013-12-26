function(add_lib NAME SRCS)
	# message("add_lib NAME=" ${NAME} " SRCS=" ${SRCS} " LIBS=" ${ARGN})

	# expand source files w.r.t. currently processed CMakeLists.txt
	# and expand object libs to cmakes' strange syntax
	expand_srcs(SRC_LIST "${SRCS}")

	# cmake requires libraries to consist of at least on source file
	LIST(LENGTH SRC_LIST SRC_LIST_SIZE)
	if(NOT SRC_LIST_SIZE)
		# message("create empty source file " ${CMAKE_BINARY_DIR} "/dummy.cpp")
		file(WRITE ${CMAKE_BINARY_DIR}/dummy.cpp "")
		set(SRC_LIST ${CMAKE_BINARY_DIR}/dummy.cpp)
	endif()

	if("${NAME}" MATCHES ".*\\.olib")
		# add the object library
		add_library(${NAME} OBJECT ${SRC_LIST})
		
		LIST(LENGTH LIB_LIST LIB_LIST_SIZE)
		if(LIB_LIST_SIZE)
			error("Cannot link libraries to object library " ${NAME})
		endif()
	else()
		# add the library
		add_library(${NAME} ${SRC_LIST})

		# link additional libraries
		LIST(LENGTH ARGN LIB_LIST_SIZE)
		if(LIB_LIST_SIZE)
			target_link_libraries(${NAME} ${ARGN})
		endif()
	endif()
endfunction(add_lib NAME SRCS)

function(add_exe NAME SRCS)
	# message("add_exe NAME=" ${NAME} " SRCS=" ${SRCS} " LIBS=" ${ARGN})

	# expand source files w.r.t. currently processed CMakeLists.txt
	# and expand object libs to cmakes' strange syntax
	expand_srcs(SRC_LIST "${SRCS}")

	# add executable with converted lib names
	add_executable(${NAME} ${SRC_LIST})

	# link additional libraries
	LIST(LENGTH ARGN LIB_LIST_SIZE)
	if(LIB_LIST_SIZE)
		target_link_libraries(${NAME} ${ARGN})
	endif()
endfunction(add_exe NAME SRCS)

function(expand_srcs LIST SRCS)
	SET(SRC_LIST)
	foreach(SRC ${SRCS})
		if("${SRC}" MATCHES ".*\\.olib")
			SET(SRC_LIST ${SRC_LIST} $<TARGET_OBJECTS:${SRC}>)
		else()
			SET(SRC_LIST ${SRC_LIST} ${CMAKE_CURRENT_LIST_DIR}/${SRC})
		endif()
	endforeach(SRC)
	# cmake's quirky way to return values
	set(${LIST} "${SRC_LIST}" PARENT_SCOPE)
endfunction(expand_srcs)

function(expand_objlibs LIST OBJLIBS)
	set(LIB_LIST)
	foreach(OBJLIB ${OBJLIBS})
		IF("${OBJLIB}" MATCHES ".*\\.olib")
			SET(LIB_LIST ${LIB_LIST} $<TARGET_OBJECTS:${OBJLIB}>)
		ELSE()
			SET(LIB_LIST ${LIB_LIST} ${OBJLIB})
		ENDIF()
	endforeach(OBJLIB)
	# cmake's quirky way to return values
	set(${LIST} "${LIB_LIST}" PARENT_SCOPE)
endfunction()