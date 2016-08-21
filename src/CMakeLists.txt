cmake_minimum_required(VERSION 2.8)
project(PwnsianCartographer)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

set (CMAKE_CXX_STANDARD 14)

file(GLOB UTILITY_SOURCES utility/*.c*)
file(GLOB ANVIL_SOURCES anvil/*.c*)
file(GLOB BLOCK_SOURCES blocks/*.c*)
file(GLOB DRAW_SOURCES draw/*.c*)
file(GLOB BASE_SOURCES *.c*)
find_package(SDL2 REQUIRED)
add_subdirectory(extlibs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} 
	${UTILITY_SOURCES} 
	${ANVIL_SOURCES} 
	${BLOCK_SOURCES} 
	${DRAW_SOURCES}
	${BASE_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC 
	${SDL2_INCLUDE_DIR} 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/extlibs/
	${PROJECT_SOURCE_DIR}/extlibs/ZipLib/Source/
) 

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} json11 nbt zip png)    
