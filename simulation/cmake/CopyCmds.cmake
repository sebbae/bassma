MACRO(copy_file in_file out_file target)
	IF(${in_file} IS_NEWER_THAN ${out_file})
		ADD_CUSTOM_COMMAND (
			TARGET ${target}
			POST_BUILD
			COMMAND	${CMAKE_COMMAND}
			ARGS -E copy ${in_file} ${out_file}
		)
	ENDIF(${in_file} IS_NEWER_THAN ${out_file})
ENDMACRO(copy_file)

MACRO(copy_file_to in_file out_dir target)
	GET_FILENAME_COMPONENT(file_name ${in_file} NAME)
	copy_file(${in_file} ${out_dir}/${file_name}
${target})	
ENDMACRO(copy_file_to)

#Copies all the files from in_file_list into the out_dir. 
# sub-trees are ignored (files are stored in same out_dir)
MACRO(copy_files_to in_file_list out_dir target)
	FOREACH(in_file ${in_file_list})
		copy_file_to(${in_file}
${out_dir} ${target})
	ENDFOREACH(in_file)	
ENDMACRO(copy_files_to)

#Copy all files and directories in in_dir to out_dir. 
# Subtrees remain intact.
MACRO(copy_dir in_dir out_dir target)
	#message("Copying directory ${in_dir}")
	FILE(GLOB_RECURSE in_file_list ${in_dir}/*)
	FOREACH(in_file ${in_file_list})
		if(NOT ${in_file} MATCHES ".*/CVS.*")
			STRING(REGEX REPLACE ${in_dir} ${out_dir} out_file ${in_file})
			copy_file(${in_file} ${out_file} ${target})
		endif(NOT ${in_file} MATCHES ".*/CVS.*")
	ENDFOREACH(in_file)	
ENDMACRO(copy_dir)