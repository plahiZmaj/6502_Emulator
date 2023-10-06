cmake_minimum_required(VERSION 3.14)

project( 6502Test )


# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.

add_subdirectory(googletest) # add googletest subdirectory

include_directories(googletest/include) # this is so we can #include <gtest/gtest.h>

# source for the test executable
set  (6502_SOURCES
		"src/test_6502.cpp"
	  )
		
source_group("src" FILES ${6502_SOURCES})
		
add_executable( 6502Test ${6502_SOURCES} 	)
add_dependencies( 6502Test 6502Lib )
target_link_libraries(6502Test gtest)
target_link_libraries(6502Test 6502Lib)