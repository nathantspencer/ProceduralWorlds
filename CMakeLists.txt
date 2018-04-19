cmake_minimum_required(VERSION 2.8)
project(ProceduralWorld)
set(CMAKE_CXX_STANDARD 14)

message("Executing gl3w_gen.py")
# execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/libs/gl3w
# WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

#======================# INCLUDES #==========================#
include_directories(libs/glfw/include)
include_directories(libs/gl3w/include)
include_directories(libs/gl3w)
include_directories(libs/glm)

#======================# EXTERNALS #=========================#
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)

#=======================# TARGETS #==========================#
file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h)
add_executable(ProceduralWorld ${SOURCES})

#======================# LINKAGE #===========================#
target_link_libraries(ProceduralWorld gl3w glfw ${OPENGL_LIBRARIES})
