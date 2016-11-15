cmake_minimum_required(VERSION 2.8)
project(PwnsianCartographer)
set(CMAKE_CXX_STANDARD 14)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

file(GLOB UTILITY_SOURCES utility/*.c*)
file(GLOB ANVIL_SOURCES anvil/*.c*)
file(GLOB BLOCK_SOURCES blocks/*.c*)
file(GLOB DRAW_SOURCES draw/*.c*)
file(GLOB BASE_SOURCES *.c*)

add_subdirectory(extlibs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Dependencies
find_package(SDL2 REQUIRED)
if (UNIX)
	find_package(PNG REQUIRED)
endif (UNIX)

add_executable(${PROJECT_NAME} 
	${UTILITY_SOURCES} 
	${ANVIL_SOURCES}
	${BLOCK_SOURCES} 
	${DRAW_SOURCES}
	${BASE_SOURCES}
)

#Set executable output in top-level folder
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME} PUBLIC 
	${SDL2_INCLUDE_DIR} 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/extlibs/
	${PROJECT_SOURCE_DIR}/extlibs/ZipLib/Source/
	${PROJECT_SOURCE_DIR}/extlibs/threadpool/src/
) 

target_link_libraries(${PROJECT_NAME} 
	${SDL2_LIBRARY} 
	${PNG_LIBRARIES} 
	json11 
	nbt 
	zip  
	docopt
)    
