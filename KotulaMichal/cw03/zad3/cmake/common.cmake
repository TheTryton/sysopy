#common functions used in project configuration/generation

function(find_c_files path found_files)
	file(GLOB_RECURSE found_files_internal "${path}/*.h" "${path}/*.c")
	set(${found_files} ${found_files_internal} PARENT_SCOPE)
endfunction()

function(find_cpp_files path found_files)
	file(GLOB_RECURSE found_files_internal "${path}/*.hpp" "${path}/*.inl" "${path}/*.cpp")
	set(${found_files} ${found_files_internal} PARENT_SCOPE)
endfunction()

function(find_c_and_cpp_files path found_files)
	find_c_files(${path} found_files_internal1)
	find_cpp_files(${path} found_files_internal2)

	set(${found_files} ${found_files_internal1} ${found_files_internal2} PARENT_SCOPE)
endfunction()
