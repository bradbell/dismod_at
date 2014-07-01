ADD_LIBRARY(cpp_lib EXCLUDE_FROM_ALL 
	get_table_column.cpp
)
SET_TARGET_PROPERTIES(cpp_lib PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
