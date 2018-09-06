# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (MarsPlus)
set(PROJECT_PATH ..)
set(DIR_SRC ${PROJECT_PATH}/src)
set(DIR_INC ${PROJECT_PATH}/include)
set(LIB_PATH ${PROJECT_PATH}/libs)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# 查找目录下的所有源文件
file(GLOB SOURCES
	${DIR_INC}/*.h
    ${DIR_SRC}/*.h
    ${DIR_SRC}/*.cpp
	${DIR_SRC}/*/*.h
    ${DIR_SRC}/*/*.cpp
)
#include search path
include_directories( ${LIB_PATH}/mars/include )
include_directories( ${DIR_INC} )
include_directories( ${DIR_SRC} )
include_directories( ${DIR_SRC}/proxy )
# 指定生成目标
add_library(MarsPlus SHARED ${SOURCES})
add_executable(demo ${PROJECT_PATH}/demo/demo.cpp)
# 添加链接库
find_library(MARSD marsd PATHS ${LIB_PATH}/mars/windows)
find_library(MARS mars PATHS ${LIB_PATH}/mars/windows)
target_link_libraries(MarsPlus debug ${MARSD} wsock32 Ws2_32)
target_link_libraries(MarsPlus optimized ${MARS} wsock32 Ws2_32)

find_library(MarsPlus marsPlus PATHS ${LIB_PATH}/mars/windows)
target_link_libraries(demo  MarsPlus )

set_target_properties(demo PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")