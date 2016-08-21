cmake_minimum_required(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_subdirectory(cnbt)
add_subdirectory(json11)
add_subdirectory(docopt-cpp)

#-------------------------------------------------#
#We need to handle ZipLib seperately.. no CMake :[
# This was done looking at the ZipLib makefile
file(GLOB_RECURSE SRC_ZLIB  ZipLib/Source/ZipLib/extlibs/zlib/*.c)
file(GLOB_RECURSE SRC_LZMA  ZipLib/Source/ZipLib/extlibs/lzma/unix/*.c)
file(GLOB_RECURSE SRC_BZIP2 ZipLib/Source/ZipLib/extlibs/bzip2/*.c)
file(GLOB_RECURSE ZIPLIB_SOURCES ZipLib/Source/ZipLib/*.cpp)
file(GLOB_RECURSE ZIPLIB_DETAIL_SOURCES ZipLib/Source/ZipLib/*.cpp)

#Internal C libraries ZipLip depends on
add_library(zip_clibs
    ${SRC_ZLIB} 
    ${SRC_LZMA} 
    ${SRC_BZIP2} 
)
target_compile_options(zip_clibs PUBLIC -fPIC -Wno-enum-conversion -O3)

#ZipLib itself
add_library(zip 
    ${ZIPLIB_SOURCES} 
    ${ZIPLIB_DETAIL_SOURCES}
)
target_compile_options(zip PUBLIC -fPIC -std=c++11 -O3)
target_link_libraries(zip zip_clibs pthread)
