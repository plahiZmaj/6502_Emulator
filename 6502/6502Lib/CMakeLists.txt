cmake_minimum_required(VERSION 3.14)

project( M6502Lib )


set  (6502_SOURCES
    "src/public/main_6502.h"
	"src/private/main_6502.cpp"
  )
		
source_group("src" FILES ${6502_SOURCES})
		
add_library( 6502Lib ${6502_SOURCES} )

target_include_directories ( 6502Lib PUBLIC "${PROJECT_SOURCE_DIR}/src/public")
target_include_directories ( 6502Lib PRIVATE "${PROJECT_SOURCE_DIR}/src/private")

#set_target_properties(6502Lib PROPERTIES FOLDER "6502Lib")