cmake_minimum_required(VERSION 3.11)
project(ProceduralWorlds)
set(CMAKE_CXX_STANDARD 14)

execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/libs/gl3w
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

#=====================================================================================#
#|----------------------------------| INCLUDES |-------------------------------------|#
#=====================================================================================#

find_package(Boost 1.67.0 COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(libs/glfw/include)
include_directories(libs/gl3w/include)
include_directories(libs/gl3w)
include_directories(libs/glm)
include_directories(sources)

#=====================================================================================#
#|--------------------------------| SOURCE GROUPS |----------------------------------|#
#=====================================================================================#

file(GLOB SOURCES sources/*.cpp sources/*.h)
file(GLOB SOURCES_COMMON sources/Common/*.cpp sources/Common/*.h)
file(GLOB SOURCES_TERRAIN sources/Terrain/*.cpp sources/Terrain/*.h)
file(GLOB SOURCES_SHADING sources/Shading/*.cpp sources/Shading/*.h)

source_group("" FILES ${SOURCES})
source_group("Common" FILES ${SOURCES_COMMON})
source_group("Terrain" FILES ${SOURCES_TERRAIN})
source_group("Shading" FILES ${SOURCES_SHADING})

#=====================================================================================#
#|---------------------------------| EXTERNALS |-------------------------------------|#
#=====================================================================================#

add_subdirectory(libs/glfw)
add_subdirectory(libs/gl3w)

#=====================================================================================#
#|-----------------------------------| TARGETS |-------------------------------------|#
#=====================================================================================#

file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h)
add_executable(ProceduralWorld ${SOURCES})

#=====================================================================================#
#|----------------------------------| LINKAGE |--------------------------------------|#
#=====================================================================================#

target_link_libraries(ProceduralWorld gl3w glfw ${OPENGL_LIBRARIES}
${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
