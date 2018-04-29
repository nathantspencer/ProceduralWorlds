cmake_minimum_required(VERSION 2.8)
project(ProceduralWorlds)
set(CMAKE_CXX_STANDARD 14)

execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/libs/gl3w
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

#=====================================================================================#
#|----------------------------------| INCLUDES |-------------------------------------|#
#=====================================================================================#

include_directories(libs/glfw/include)
include_directories(libs/gl3w/include)
include_directories(libs/gl3w)
include_directories(libs/glm)

#=====================================================================================#
#|--------------------------------| SOURCE GROUPS |----------------------------------|#
#=====================================================================================#

file(GLOB SOURCES sources/*.cpp sources/*.h)
file(GLOB SOURCES_COMMON sources/Common/*.cpp sources/Common/*.h)
file(GLOB SOURCES_TERRAIN sources/Terrain/*.cpp sources/Terrain/*.h)

source_group("" FILES ${SOURCES})
source_group("Common" FILES ${SOURCES_COMMON})
source_group("Terrain" FILES ${SOURCES_TERRAIN})

#=====================================================================================#
#|---------------------------------| EXTERNALS |-------------------------------------|#
#=====================================================================================#

add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)

#=====================================================================================#
#|-----------------------------------| TARGETS |-------------------------------------|#
#=====================================================================================#

file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h)
add_executable(ProceduralWorld ${SOURCES})

#=====================================================================================#
#|----------------------------------| LINKAGE |--------------------------------------|#
#=====================================================================================#

target_link_libraries(ProceduralWorld gl3w glfw ${OPENGL_LIBRARIES})
